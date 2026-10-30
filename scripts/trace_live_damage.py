#!/usr/bin/env python3
"""Trace the DllBattle damage calculation call by call in a running game.

On entry to the damage function it records the register and stack arguments
and the memory the function reads directly, then arms breakpoints on every
call site inside the function (scripts/battle_damage_sites.json). Each call
records r0-r3 and two stack words before the call and r0 after it. At return
it records the 16-bit damage written to the output. Memory is only read; all
breakpoints are removed before detaching. Static addresses are converted with
the module delta (runtime = static + delta) confirmed by comparing code bytes.
"""
import argparse
import json
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from capture_live_calls import Stub  # noqa: E402


def main():
    root = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=24689)
    parser.add_argument('--seconds', type=float, default=240)
    parser.add_argument('--max-calls', type=int, default=20)
    parser.add_argument('--output', required=True)
    args = parser.parse_args()
    spec = json.loads((root / 'battle_damage_sites.json').read_text())
    delta = int(spec['module_delta'], 16)
    entry = (int(spec['function'], 16) + delta) & 0xffffffff
    sites = {(int(s['site'], 16) + delta) & 0xffffffff: s for s in spec['sites']}
    returns = {site + 4: site for site in sites}
    random_literal = (0x1086a6dc + delta) & 0xffffffff
    stub = Stub(args.host, args.port)
    inserted = set()

    def arm(address):
        if address not in inserted:
            stub.breakpoint(address, True)
            inserted.add(address)

    def disarm(address):
        if address in inserted:
            stub.breakpoint(address, False)
            inserted.discard(address)

    def step_over(address):
        disarm(address)
        stub.packet('s')
        arm(address)

    calls = 0
    deadline = time.monotonic() + args.seconds
    with open(args.output, 'w') as out:
        try:
            arm(entry)
            current = None
            while calls < args.max_calls and time.monotonic() < deadline:
                stop = stub.resume(max(1.0, deadline - time.monotonic()))
                if stop is None:
                    break
                r = stub.registers()
                pc, sp, lr = r[15], r[13], r[14] & ~1
                if pc == entry and current is None:
                    stack = struct.unpack('<6I', stub.read(sp, 24))
                    ctx, attacker, defender, move = r[0], r[1], r[2], r[3]
                    rules_object = stub.u32(ctx + 4)
                    field = stub.u32(ctx + 0x1c)
                    owner = stub.u32(random_literal)
                    current = {'entry_sp': sp, 'return': lr, 'args': r[:4], 'stack': list(stack),
                               'attacker_id': stub.read(attacker + 0x19, 1)[0],
                               'defender_id': stub.read(defender + 0x19, 1)[0],
                               'move': stub.read(move, 8).hex(), 'rules_object': rules_object,
                               'rules_word': stub.u32(rules_object + 0x10), 'field': field,
                               'weather': stub.read(field + 1, 1)[0], 'random_owner': owner,
                               'generator': stub.u32(owner + 0x54), 'calls': []}
                    disarm(entry)
                    for address in list(sites) + list(returns):
                        arm(address)
                    arm(lr)
                    continue
                if current is None:
                    step_over(pc)
                    continue
                if pc in sites:
                    current['calls'].append({'site': sites[pc]['site'], 'target': sites[pc]['target'],
                                             'args': r[:4], 'stack': list(struct.unpack('<2I', stub.read(sp, 8)))})
                    step_over(pc)
                elif pc in returns:
                    current['calls'][-1]['result'] = r[0]
                    if sites[returns[pc]]['target'] == '0x108202d8':  # attack() writes the burn-exception flag
                        current['calls'][-1]['flag'] = stub.read(current['entry_sp'] - 0x28, 1)[0]
                    step_over(pc)
                elif pc == current['return'] and sp == current['entry_sp']:
                    output = current['stack'][5]
                    current['result'] = r[0]
                    current['damage'] = struct.unpack('<H', stub.read(output, 2))[0]
                    out.write(json.dumps(current) + '\n')
                    out.flush()
                    calls += 1
                    for address in list(sites) + list(returns) + [current['return']]:
                        disarm(address)
                    current = None
                    arm(entry)
                else:
                    step_over(pc)
        finally:
            for address in list(inserted):
                try:
                    stub.breakpoint(address, False)
                except Exception:
                    pass
            stub.sock.sendall(b'$D#44')
            stub.sock.close()
    print(json.dumps({'damage_calls': calls}))


if __name__ == '__main__':
    main()
