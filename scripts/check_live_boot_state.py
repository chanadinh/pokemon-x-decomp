#!/usr/bin/env python3
"""Compare a running game's boot-time resource state with the reconstruction.

Connects read-only to an emulator GDB stub (Azahar/Citra-style, default port
24689), reads the resource cells, configuration table, archive member tables,
loaded tables and message resources, then compares them with the results the
reconstructed boot, archive-index and message-construction milestones produce
from the same original files. Only memory reads (`m`) and a final detach are
sent; nothing is written to the game.
"""
import argparse
import json
import socket
import struct
import sys
from pathlib import Path

CODE_SHA256 = '0f6d3e1d66cbcd55d46fa453db4b6aa489e03c3537c6b24026f2a53af142797a'


class Stub:
    def __init__(self, host, port):
        self.sock = socket.create_connection((host, port), timeout=20)
        self.buffer = b''
        self.packet('qSupported:swbreak+')
        self.packet('?')

    def _receive(self):
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

    def packet(self, body):
        data = body.encode('latin-1')
        self.sock.sendall(b'$' + data + b'#' + b'%02x' % (sum(data) & 0xff))
        return self._receive()

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

    def detach(self):
        try:
            self.sock.sendall(b'$D#44')
        finally:
            self.sock.close()


class Garc:
    def __init__(self, path):
        self.bytes = path.read_bytes()
        self.fato = struct.unpack_from('<I', self.bytes, 4)[0]
        self.entries = struct.unpack_from('<H', self.bytes, self.fato + 8)[0]
        self.fatb = self.fato + struct.unpack_from('<I', self.bytes, self.fato + 4)[0]
        self.data = struct.unpack_from('<I', self.bytes, 0x10)[0]

    def record(self, entry, selector):
        rec = self.fatb + 12 + struct.unpack_from('<I', self.bytes, self.fato + 12 + 4 * entry)[0]
        bits = struct.unpack_from('<I', self.bytes, rec)[0]
        slot = bin(bits & ((1 << selector) - 1)).count('1') if selector < 32 and bits >> selector & 1 else 0
        return struct.unpack_from('<III', self.bytes, rec + 4 + 12 * slot)

    def rows(self, selector):
        """Independent resident rows: padded span, record length, absolute offset."""
        return [[end - start, length, self.data + start]
                for start, end, length in (self.record(e, selector) for e in range(self.entries))]

    def member(self, entry):
        start, end, _ = self.record(entry, 0)
        return self.bytes[self.data + start:self.data + end]


def rodata_string(code, mapping, address):
    for section in mapping:
        start = int(section['address'], 16)
        if section['code_offset'] is not None and start <= address < start + section['size']:
            offset = section['code_offset'] + address - start
            return code[offset:offset + 256].decode('utf-16-le', 'replace').split('\0')[0]
    raise ValueError('address %#x is not file-backed' % address)


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=24689)
    parser.add_argument('--romfs', type=Path, default=root / 'inputs/romfs')
    parser.add_argument('--code', type=Path, default=root / 'inputs/pokemon-x-0f6d3e1d66cb/code.bin')
    parser.add_argument('--target', type=Path, default=root / 'outputs/rea-live/target.json')
    parser.add_argument('--json', type=Path, help='write the comparison summary here')
    args = parser.parse_args()
    import hashlib
    code = args.code.read_bytes()
    if hashlib.sha256(code).hexdigest() != CODE_SHA256:
        parser.exit(1, 'error: code.bin differs from the pinned input\n')
    mapping = json.loads(args.target.read_text())['mapping']
    results = []

    def check(name, ok):
        results.append({'check': name, 'match': bool(ok)})
        print('%-56s %s' % (name, 'MATCH' if ok else 'DIFFERS'))

    stub = Stub(args.host, args.port)
    try:
        for address in (0x100000, 0x112368, 0x127ccc, 0x167db0, 0x174c0c, 0x300000):
            check('running code at %08x' % address,
                  stub.read(address, 0x200) == code[address - 0x100000:address - 0x100000 + 0x200])
        config = stub.u32(0x5d1870)
        words = struct.unpack('<24I', stub.read(config, 96))
        paths = {i: rodata_string(code, mapping, words[i]) for i in (0, 1, 2, 3, 4, 5, 6, 12, 14, 15, 17)}
        for i, path in paths.items():
            print('configuration word %2d -> %s' % (i, path))
        local = {i: args.romfs / p.split(':/', 1)[-1].lstrip('/') for i, p in paths.items()}
        selector = stub.u32(0x5d1124)
        print('GARC selector word 005d1124 = %d; language word 005d113c = %d' % (selector, stub.u32(0x5d113c)))
        archives = {'personal': (0x5d1778, 0, False), 'learnset': (0x5d17ec, 1, True), 'growth': (0x5e52b0, 2, True),
                    'word 3': (0x5e5288, 3, True), 'word 4': (0x5d1760, 4, True), 'tail': (0x5d17d0, 6, False),
                    'moves': (0x5e5338, 12, True)}
        for name, (cell, word, resident) in archives.items():
            archive = stub.u32(cell)
            head = stub.read(archive, 0x78)
            flag = struct.unpack_from('<i', head, 0x4c)[0]
            count = struct.unpack_from('<H', head, 0x34)[0]
            table = struct.unpack_from('<I', head, 0x3c)[0]
            garc = Garc(local[word])
            if not resident:
                check('archive %s has no resident table' % name, flag < 0 and count == 0 and table == 0)
                continue
            live = struct.unpack('<%dI' % (3 * count), stub.read(table, 12 * count))
            check('archive %s member table (%d rows)' % (name, count),
                  flag >= 0 and [list(live[3 * i:3 * i + 3]) for i in range(count)] == garc.rows(selector))
        personal = Garc(local[0])
        flat = personal.member(personal.entries - 1)
        check('personal table (%d bytes)' % len(flat), stub.read(stub.u32(0x5d177c), len(flat)) == flat)
        tail = Garc(local[6])
        block = tail.member(tail.entries - 1)
        check('final-member block (%d bytes)' % len(block), stub.read(stub.u32(0x5d17d4), len(block)) == block)
        cache = stub.read(stub.u32(0x5d17b0), 12)
        check('growth cache vtable and capacity',
              struct.unpack_from('<I', cache, 0)[0] == 0x5989e0 and cache[8] == words[8] & 0xff)
        expected_messages = {0x5d1794: (1, words[0x12]), 0x5d1798: (0, words[0x13]),
                             0x5d179c: (0, words[0x14]), 0x5d1888: (1, words[0x15])}
        for cell, (mode, member) in expected_messages.items():
            m = stub.read(stub.u32(cell), 0x28)
            vtable, mode_byte = struct.unpack_from('<I', m, 0)[0], m[0x0c]
            member_word, section = struct.unpack_from('<I', m, 0x10)[0], struct.unpack_from('<I', m, 0x1c)[0]
            check('message resource %08x (mode %d, member %#x)' % (cell, mode, member),
                  vtable == 0x5985b4 and mode_byte == mode and member_word == member and (section != 0) == bool(mode))
    finally:
        stub.detach()
    matched = sum(r['match'] for r in results)
    print('%d of %d checks match' % (matched, len(results)))
    if args.json:
        args.json.write_text(json.dumps({'paths': paths, 'selector': selector, 'results': results}, indent=2) + '\n')
    return 0 if matched == len(results) else 1


if __name__ == '__main__':
    sys.exit(main())
