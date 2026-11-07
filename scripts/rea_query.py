#!/usr/bin/env python3
"""Queue REA MCP requests to a running daemon session and wait for the answers.

A daemon (one warm Ghidra session) takes JSON jobs from a queue directory in
numeric order and writes NNN-<tool>-request.json / -response.json into its
session directory. Several workers may share it: each job is matched to its
response by tool name and arguments, so callers never need the sequence
numbers.

  rea_query.py analyze 0x1081d558 0x1081dd1c      # analyze_function, detail complete
  rea_query.py read 0x1081d558:284 0x1081dd1c:480  # read_bytes
  rea_query.py asm 0x1081d558                      # print the listing (fault idiom collapsed)

The queue and session default to $REA_QUEUE and $REA_SESSION. Queue every
job first and wait once: after the first query of a session, each answer
takes well under a second.
"""
import argparse
import json
import os
import sys
import time
from pathlib import Path


def requests_after(session, start):
    for p in session.glob('*-request.json'):
        seq = int(p.name.split('-', 1)[0])
        if seq > start:
            yield seq, p


def latest(session):
    return max((int(p.name.split('-', 1)[0]) for p in session.glob('*-request.json')), default=0)


def submit(queue, jobs):
    stamp = time.time_ns()
    for i, (name, arguments) in enumerate(jobs):
        path = queue / f'{stamp + i}.json'
        tmp = queue / f'.{stamp + i}.tmp'
        tmp.write_text(json.dumps({'name': name, 'arguments': arguments}) + '\n', newline='\n')
        tmp.rename(path)


def wait(session, jobs, start, timeout):
    """Response paths for each job, matched by tool name and arguments."""
    pending = {i: job for i, job in enumerate(jobs)}
    found = {}
    deadline = time.time() + timeout
    while pending:
        if (session / 'stopped').exists():
            raise SystemExit('REA daemon stopped: ' + (session / 'last.txt').read_text().strip())
        for seq, request in sorted(requests_after(session, start)):
            response = request.with_name(request.name.replace('-request.json', '-response.json'))
            if not response.exists():
                continue
            params = json.loads(request.read_text()).get('params', {})
            for i, (name, arguments) in list(pending.items()):
                if params.get('name') == name and params.get('arguments') == arguments:
                    found[i] = response
                    del pending[i]
        if pending:
            if time.time() > deadline:
                raise SystemExit(f'timed out waiting for {len(pending)} REA answers')
            time.sleep(0.5)
    return [found[i] for i in range(len(jobs))]


def evidence(response):
    result = json.loads(response.read_text())['result']
    if result.get('isError'):
        raise SystemExit(f'{response.name}: ' + result['content'][0]['text'][:500])
    return result.get('structuredContent') or json.loads(result['content'][0]['text'])


def listing(response):
    n = evidence(response)['normalized_result']
    lines, out, i = n['assembly'], [], 0
    while i < len(lines):
        if i + 3 < len(lines) and lines[i].endswith('mov r2, #0x0') and lines[i + 1].endswith('cpy r1, r2') \
                and lines[i + 2].endswith('cpy r0, r2') and lines[i + 3].endswith('bl 0x10800394'):
            out.append(lines[i].split(':')[0] + ': FAULT')
            i += 4
            continue
        out.append(lines[i])
        i += 1
    body = n['procedure']['body']
    return f"{n['procedure']['name']} {body['total_bytes']} bytes ranges {body['ranges']}\n" + '\n'.join(out)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--queue', type=Path, default=os.environ.get('REA_QUEUE'))
    parser.add_argument('--session', type=Path, default=os.environ.get('REA_SESSION'))
    parser.add_argument('--timeout', type=float, default=1800)
    parser.add_argument('mode', choices=['analyze', 'read', 'asm'])
    parser.add_argument('targets', nargs='+')
    args = parser.parse_args()
    if not args.queue or not args.session:
        parser.error('set --queue/--session or $REA_QUEUE/$REA_SESSION')
    if args.mode == 'read':
        jobs = [('read_bytes', {'address': t.split(':')[0], 'length': int(t.split(':')[1], 0)}) for t in args.targets]
    else:
        jobs = [('analyze_function', {'procedure': t, 'detail': 'complete'}) for t in args.targets]
    for _, arguments in jobs:
        for key in ('address', 'procedure'):
            if key in arguments:
                arguments[key] = '0x%x' % int(arguments[key], 16)
    start = latest(args.session)
    submit(args.queue, jobs)
    responses = wait(args.session, jobs, start, args.timeout)
    for response in responses:
        if args.mode == 'asm':
            print(listing(response))
            print()
        else:
            evidence(response)
            print(response)


if __name__ == '__main__':
    main()
