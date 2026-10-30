#!/usr/bin/env python3
"""Record who calls selected main-executable routines in a running game.

At each breakpoint hit it records r0-r3, the return address, 64 bytes of code
around the return address and 64 stack words, then steps over the breakpoint
and continues. Memory is only read; breakpoints are removed before detaching.
scripts/map_live_callers.py maps return addresses inside CRO modules back to
static-analysis addresses by matching the recorded code bytes.
"""
import argparse
import json
import struct
import sys
import time

sys.path.insert(0, str(__import__('pathlib').Path(__file__).resolve().parent))
from capture_live_calls import Stub  # noqa: E402


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=24689)
    parser.add_argument('--address', action='append', required=True, help='routine entry (hex), repeatable')
    parser.add_argument('--seconds', type=float, default=180)
    parser.add_argument('--max-hits', type=int, default=400)
    parser.add_argument('--output', required=True)
    args = parser.parse_args()
    targets = [int(a, 16) for a in args.address]
    stub = Stub(args.host, args.port)
    hits = 0
    deadline = time.monotonic() + args.seconds
    with open(args.output, 'w') as out:
        try:
            for t in targets:
                stub.breakpoint(t, True)
            while hits < args.max_hits and time.monotonic() < deadline:
                stop = stub.resume(max(1.0, deadline - time.monotonic()))
                if stop is None:
                    break
                regs = stub.registers()
                pc, sp, lr = regs[15], regs[13], regs[14] & ~1
                if pc not in targets:
                    continue
                record = {'target': '%08x' % pc, 'args': regs[:4], 'lr': lr, 'sp': sp,
                          'code': stub.read(lr - 32, 64).hex(), 'stack': stub.read(sp, 256).hex()}
                out.write(json.dumps(record) + '\n')
                out.flush()
                hits += 1
                stub.breakpoint(pc, False)
                stub.packet('s')
                stub.breakpoint(pc, True)
        finally:
            for t in targets:
                try:
                    stub.breakpoint(t, False)
                except Exception:
                    pass
            stub.sock.sendall(b'$D#44')
            stub.sock.close()
    print(json.dumps({'hits': hits}))


if __name__ == '__main__':
    main()
