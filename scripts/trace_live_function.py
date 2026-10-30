#!/usr/bin/env python3
"""Trace CRO functions call by call in a running game from a spec file.

The spec (for example scripts/battle_stats_sites.json) lists functions with
static entry addresses, internal call sites, stack-argument counts, memory to
read at entry and optional memory to read at return. Read expressions are
`BASE+OFFSET:SIZE`, where BASE is a register (r0..r3), a stack argument (s0..)
or `[EXPR]` for the word at EXPR. On entry the tracer arms every call site; each
call records r0-r3 and two stack words before it and r0 after it. Memory is only
read and every breakpoint is removed before detaching. Runtime addresses are
static addresses plus the spec's module delta.
"""
import argparse
import json
import re
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from capture_live_calls import Stub  # noqa: E402


def evaluate(stub, expression, regs, stack):
    match = re.fullmatch(r'\[(.+)\]', expression)
    if match:
        return stub.u32(evaluate(stub, match.group(1), regs, stack))
    base, _, offset = expression.partition('+')
    if base.startswith('['):
        raise ValueError(expression)
    value = regs[int(base[1:])] if base.startswith('r') else stack[int(base[1:])]
    return (value + (int(offset, 0) if offset else 0)) & 0xffffffff


def read_list(stub, reads, regs, stack):
    out = {}
    for item in reads:
        expression, size = item.rsplit(':', 1)
        if expression.startswith('['):
            close = expression.index(']')
            address = evaluate(stub, expression[:close + 1], regs, stack)
            rest = expression[close + 1:]
            address = (address + (int(rest[1:], 0) if rest else 0)) & 0xffffffff
        else:
            address = evaluate(stub, expression, regs, stack)
        out[item] = {'address': address, 'bytes': stub.read(address, int(size, 0)).hex()}
    return out


def main():
    root = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--spec', type=Path, required=True)
    parser.add_argument('--host', default='127.0.0.1')
    parser.add_argument('--port', type=int, default=24689)
    parser.add_argument('--seconds', type=float, default=240)
    parser.add_argument('--max-calls', type=int, default=20)
    parser.add_argument('--output', required=True)
    args = parser.parse_args()
    spec = json.loads((args.spec if args.spec.is_absolute() else Path.cwd() / args.spec).read_text())
    delta = int(spec['module_delta'], 16)
    runtime = lambda a: (int(a, 16) + delta) & 0xffffffff
    functions = {runtime(f['entry']): f for f in spec['functions']}
    stub = Stub(args.host, args.port)
    inserted = set()

    def arm(a):
        if a not in inserted:
            stub.breakpoint(a, True); inserted.add(a)

    def disarm(a):
        if a in inserted:
            stub.breakpoint(a, False); inserted.discard(a)

    def step_over(a):
        disarm(a); stub.packet('s'); arm(a)

    calls, current = 0, None
    deadline = time.monotonic() + args.seconds
    with open(args.output, 'w') as out:
        try:
            for entry in functions:
                arm(entry)
            while calls < args.max_calls and time.monotonic() < deadline:
                if stub.resume(max(1.0, deadline - time.monotonic())) is None:
                    break
                r = stub.registers()
                pc, sp, lr = r[15], r[13], r[14] & ~1
                if current is None and pc in functions:
                    f = functions[pc]
                    stack = list(struct.unpack('<%dI' % f['stack_words'], stub.read(sp, 4 * f['stack_words'])))
                    sites = {runtime(s['site']): s for s in f['sites']}
                    current = {'function': f['name'], 'entry': f['entry'], 'entry_sp': sp, 'return': lr,
                               'args': r[:4], 'stack': stack, 'reads': read_list(stub, f['reads'], r, stack),
                               'calls': [], '_sites': sites, '_exit': f.get('exit_reads', [])}
                    for e in functions:
                        disarm(e)
                    for a in list(sites) + [a + 4 for a in sites] + [lr]:
                        arm(a)
                    continue
                if current is None:
                    step_over(pc); continue
                sites = current['_sites']
                handled = False
                # A return address can also be the next call site: record the result first.
                if pc - 4 in sites and current['calls'] and 'result' not in current['calls'][-1]:
                    current['calls'][-1]['result'] = r[0]
                    handled = True
                if pc in sites:
                    current['calls'].append({'site': sites[pc]['site'], 'target': sites[pc]['target'], 'args': r[:4],
                                             'stack': list(struct.unpack('<2I', stub.read(sp, 8)))})
                    handled = True
                if handled:
                    step_over(pc)
                elif pc == current['return'] and sp == current['entry_sp']:
                    current['result'] = r[0]
                    current['exit'] = read_list(stub, current['_exit'], current['args'], current['stack'])
                    for a in list(sites) + [a + 4 for a in sites] + [current['return']]:
                        disarm(a)
                    record = {k: v for k, v in current.items() if not k.startswith('_')}
                    out.write(json.dumps(record) + '\n'); out.flush()
                    calls += 1; current = None
                    for e in functions:
                        arm(e)
                else:
                    step_over(pc)
        finally:
            for a in list(inserted):
                try:
                    stub.breakpoint(a, False)
                except Exception:
                    pass
            stub.sock.sendall(b'$D#44'); stub.sock.close()
    print(json.dumps({'calls': calls}))


if __name__ == '__main__':
    main()
