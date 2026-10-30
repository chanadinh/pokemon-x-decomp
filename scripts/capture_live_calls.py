#!/usr/bin/env python3
"""Capture calls to reconstructed routines from a running game for replay.

Sets software breakpoints at selected routine entries through an emulator GDB
stub (default 127.0.0.1:24689), records arguments and the memory each routine
reads, then catches the matching return (a temporary breakpoint at the return
address with the same stack pointer) and records the result. Memory is only
read. All breakpoints are removed before detaching. Captures are JSON lines
for scripts/replay_live_calls.c.
"""
import argparse
import json
import socket
import struct
import sys
import time


class Stub:
    def __init__(self, host, port):
        self.sock = socket.create_connection((host, port), timeout=20)
        self.buffer = b''
        self.packet('qSupported:swbreak+')
        self.packet('?')

    def _receive(self, timeout=20):
        self.sock.settimeout(timeout)
        while True:
            start = self.buffer.find(b'$')
            end = self.buffer.find(b'#', start) if start >= 0 else -1
            if end >= 0 and len(self.buffer) >= end + 3:
                body = self.buffer[start + 1:end]
                self.buffer = self.buffer[end + 3:]
                self.sock.sendall(b'+')
                return body.decode('latin-1')
            chunk = self.sock.recv(65536)
            if not chunk:
                raise ConnectionError('debug stub closed the connection')
            self.buffer += chunk

    def send(self, body):
        data = body.encode('latin-1')
        self.sock.sendall(b'$' + data + b'#' + b'%02x' % (sum(data) & 0xff))

    def packet(self, body, timeout=20):
        self.send(body)
        return self._receive(timeout)

    def read(self, address, size):
        out = bytearray()
        while size:
            n = min(size, 0x800)
            reply = self.packet('m%x,%x' % (address, n))
            if len(reply) != 2 * n:
                raise IOError('read %#x+%#x failed: %s' % (address, n, reply[:20]))
            out += bytes.fromhex(reply)
            address += n
            size -= n
        return bytes(out)

    def u32(self, address):
        return struct.unpack('<I', self.read(address, 4))[0]

    def registers(self):
        reply = self.packet('g')
        return [struct.unpack('<I', bytes.fromhex(reply[8 * i:8 * i + 8]))[0] for i in range(16)]

    def breakpoint(self, address, insert):
        reply = self.packet('%s0,%x,4' % ('Z' if insert else 'z', address))
        if reply != 'OK':
            raise IOError('breakpoint %s at %#x failed: %r' % ('insert' if insert else 'remove', address, reply))

    def resume(self, timeout):
        """Continue until a stop reply; None if the window ends first."""
        self.send('c')
        try:
            return self._receive(timeout)
        except socket.timeout:
            self.sock.sendall(b'\x03')  # interrupt
            self._receive(20)
            return None


def personal_entry(stub, r):
    record = stub.read(r[0], 12)
    return {'record': record.hex(), 'table': stub.u32(0x5d177c)}

def personal_exit(stub, entry_regs, r):
    record = stub.read(entry_regs[0], 12)
    payload = stub.read(struct.unpack_from('<I', record, 8)[0], 64)
    return {'record': record.hex(), 'payload': payload.hex()}

def field_entry(stub, r):
    record = stub.read(r[0], 12)
    return {'record': record.hex(), 'payload': stub.read(struct.unpack_from('<I', record, 8)[0], 64).hex()}

def growth_entry(stub, r):
    return {}

def growth_exit(stub, entry_regs, r):
    record = stub.read(stub.u32(0x5d17ac), 0x19c)
    return {'growth_record': record.hex()}

def nothing(stub, entry_regs, r):
    return {}

def stack_entry(stub, r):
    """Fifth argument from the stack (EV for HP, nature for stats)."""
    return {'stack': stub.u32(r[13])}

def stat_entry(stub, r):
    nature = stub.u32(r[13])
    row = 0x00570f6c + 5 * nature
    return {'stack': nature, 'nature_row': stub.read(row & 0xffffffff, 5).hex() if nature < 0x1000 else ''}

TARGETS = {
    0x00174c0c: ('load_personal_record', 3, personal_entry, personal_exit),
    0x00175c78: ('personal_field', 2, field_entry, nothing),
    0x00167db0: ('select_growth', 2, growth_entry, growth_exit),
    0x00168b3c: ('level_from_growth', 3, growth_entry, nothing),
    0x00168b84: ('hp_value', 4, stack_entry, nothing),
    0x0014ffa4: ('attack_value', 4, stat_entry, nothing),
    0x0014ffd4: ('defense_value', 4, stat_entry, nothing),
    0x0014ff74: ('speed_value', 4, stat_entry, nothing),
    0x00150004: ('special_attack_value', 4, stat_entry, nothing),
    0x00150034: ('special_defense_value', 4, stat_entry, nothing),
    0x00168bbc: ('move_value', 2, growth_entry, nothing),
    0x00150220: ('maximum_pp', 2, growth_entry, nothing),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=24689)
    parser.add_argument('--seconds', type=float, default=120)
    parser.add_argument('--per-routine', type=int, default=40)
    parser.add_argument('--output', required=True)
    parser.add_argument('--distinct', action='store_true', help='skip calls whose arguments were already recorded')
    parser.add_argument('--only', help='comma-separated routine names to capture (default: all)')
    args = parser.parse_args()
    stub = Stub(args.host, args.port)
    active = {a for a, t in TARGETS.items() if not args.only or t[0] in args.only.split(',')}
    counts = {TARGETS[a][0]: 0 for a in active}
    pending = {}  # return address -> list of (sp, entry address, entry regs, entry data)
    inserted = set()
    seen = set()

    def set_bp(address):
        if address not in inserted:
            stub.breakpoint(address, True)
            inserted.add(address)

    def clear_bp(address):
        if address in inserted:
            stub.breakpoint(address, False)
            inserted.discard(address)

    deadline = time.monotonic() + args.seconds
    with open(args.output, 'w') as out:
        try:
            for address in active:
                set_bp(address)
            while time.monotonic() < deadline and (active or pending):
                stop = stub.resume(max(1.0, deadline - time.monotonic()))
                if stop is None:
                    break
                regs = stub.registers()
                pc, sp, lr = regs[15], regs[13], regs[14]
                if pc in pending:
                    waiting = pending[pc]
                    match = next((w for w in waiting if w[0] == sp), None)
                    if not match and pc not in TARGETS:
                        # Another caller reached this return address: step over it.
                        clear_bp(pc)
                        stub.packet('s')
                        set_bp(pc)
                        continue
                    if match:
                        waiting.remove(match)
                        _, entry, entry_regs, entry_data = match
                        name, argc, _, on_exit = TARGETS[entry]
                        # Personal routines are keyed by species/form or selector, not record address.
                        key = (name, tuple(entry_regs[1:argc]) if name.startswith('load') or name == 'personal_field'
                               else tuple(entry_regs[:argc]))
                        if not (args.distinct and key in seen):
                            seen.add(key)
                            record = {'routine': name, 'address': '%08x' % entry,
                                      'args': entry_regs[:argc], 'entry': entry_data,
                                      'result': regs[0], 'exit': on_exit(stub, entry_regs, regs)}
                            out.write(json.dumps(record) + '\n')
                            out.flush()
                            counts[name] += 1
                        if counts[name] >= args.per_routine and entry in active:
                            active.discard(entry)
                            if entry not in pending:
                                clear_bp(entry)
                    if not waiting:
                        del pending[pc]
                        if pc not in active:
                            clear_bp(pc)
                    if pc not in TARGETS:
                        continue
                if pc in TARGETS and pc in active:
                    name, argc, on_entry, _ = TARGETS[pc]
                    data = on_entry(stub, regs)
                    pending.setdefault(lr & ~1, []).append((sp, pc, regs, data))
                    set_bp(lr & ~1)
                    # Step past the entry breakpoint: remove it until the return is seen.
                    clear_bp(pc)
                    # Re-arm entries whose call has returned on later stops.
                for address in list(active):
                    if address not in inserted and not any(w[1] == address for ws in pending.values() for w in ws):
                        set_bp(address)
        finally:
            for address in list(inserted):
                try:
                    clear_bp(address)
                except Exception:
                    pass
            stub.sock.sendall(b'$D#44')
            stub.sock.close()
    print(json.dumps(counts))
    return 0


if __name__ == '__main__':
    sys.exit(main())
