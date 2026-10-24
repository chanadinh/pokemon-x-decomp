"""Independent check of story text, zone scripts and NPC dialogue.

1. Decodes every story-text file of a/0/8/2 (English) itself (pk3DS 6daaca9
   TextFile layout and key schedule) and renders every line with the
   viewer's rules ('\\n', '\\f' at scroll/clear, [NAME] variables, TRNAME as
   the player name); compares all lines with the native renderer through
   dump_talk.
2. Decodes every zone's section-2 script itself (Pawn 3.3 AMX header with
   name hashes, GF 7-bit word encoding) and runs every NPC's event on its
   own Pawn 3.3 interpreter with the same native policy (see
   assets/zone_talk.h); compares result, instruction count, yields, native
   calls and message lines with the native interpreter for all 2,524 NPCs,
   and checks that every message line exists in the zone's text file.
3. Optionally runs the viewer: the house NPC's --inspect line and a scripted
   talk with its pages and a capture of the open window.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import subprocess

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


zone_check = load('zone_check', '../bch-zone/verify_zone.py')
walk = zone_check.walk
member, garc_count, parse_entities = zone_check.member, zone_check.garc_count, zone_check.parse_entities

# ---------------------------------------------------------------- text
NAMES = {0xFF00: 'COLOR', 0x0100: 'TRNAME', 0x0101: 'PKNAME', 0x0102: 'PKNICK', 0x0103: 'TYPE',
         0x0105: 'LOCATION', 0x0106: 'ABILITY', 0x0107: 'MOVE', 0x0108: 'ITEM1', 0x0109: 'ITEM2',
         0x010A: 'sTRBAG', 0x010B: 'BOX', 0x010D: 'EVSTAT', 0x0110: 'OPOWER', 0x0127: 'RIBBON',
         0x0134: 'MIINAME', 0x013E: 'WEATHER', 0x0189: 'TRNICK', 0x018A: '1stchrTR', 0x018B: 'SHOUTOUT',
         0x018E: 'BERRY', 0x018F: 'REMFEEL', 0x0190: 'REMQUAL', 0x0191: 'WEBSITE', 0x019C: 'CHOICECOS',
         0x01A1: 'GSYNCID', 0x0192: 'PRVIDSAY', 0x0193: 'BTLTEST', 0x0195: 'GENLOC', 0x0199: 'CHOICEFOOD',
         0x019A: 'HOTELITEM', 0x019B: 'TAXISTOP', 0x019F: 'MAISTITLE', 0x1000: 'ITEMPLUR0',
         0x1001: 'ITEMPLUR1', 0x1100: 'GENDBR', 0x1101: 'NUMBRNCH', 0x1302: 'iCOLOR2', 0x1303: 'iCOLOR3',
         0x0200: 'NUM1', 0x0201: 'NUM2', 0x0202: 'NUM3', 0x0203: 'NUM4', 0x0204: 'NUM5', 0x0205: 'NUM6',
         0x0206: 'NUM7', 0x0207: 'NUM8', 0x0208: 'NUM9'}
REMAP = {0xE08D: 0x2026, 0xE08E: 0x2642, 0xE08F: 0x2640, 0xE07F: 0x202F}


def text_lines(d):
    sections, count, total, key0, section = struct.unpack_from('<HHIII', d, 0)
    assert sections == 1 and key0 == 0 and section + total == len(d)
    assert struct.unpack_from('<I', d, section)[0] == total
    lines, key = [], 0x7C89
    for i in range(count):
        offset, length = struct.unpack_from('<IH', d, section + 4 + 8 * i)
        k, units = key, []
        for j in range(length):
            units.append(struct.unpack_from('<H', d, section + offset + 2 * j)[0] ^ k)
            k = ((k << 3) | (k >> 13)) & 0xFFFF
        lines.append(units)
        key = (key + 0x2983) & 0xFFFF
    return lines


def render(units, player='PLAYER'):
    out, i, n = [], 0, len(units)
    while i < n:
        c = units[i]
        if c == 0:
            break
        if c == 0x10:
            if i + 1 >= n or units[i + 1] == 0 or i + 1 + units[i + 1] >= n:
                return None
            count, code = units[i + 1], units[i + 2]
            i += 2 + count
            if code in (0xBE00, 0xBE01):
                out.append('\f')
                if i < n and units[i] == ord('\n'):
                    i += 1
                continue
            if code in (0xBE02, 0xBDFF, 0xFF00, 0x1302, 0x1303):
                continue
            out.append(player if code == 0x0100 else '[%s]' % NAMES[code] if code in NAMES else '[%04X]' % code)
            continue
        out.append(chr(REMAP.get(c, c)))
        i += 1
    return ''.join(out)


# ---------------------------------------------------------------- scripts
def decode_script(d):
    size, magic, fv, av, flags, defsize = struct.unpack_from('<IHBBHH', d, 0)
    assert magic == 0xF1E0 and fv == av == 10 and defsize == 8
    cod, dat, hea, stp, cip, publics, natives, libraries, pubvars, tags, nametable = struct.unpack_from('<11I', d, 12)
    entries = lambda a, b: [struct.unpack_from('<II', d, o) for o in range(a, b, 8)]
    words, at = [], cod
    while len(words) < (hea - cod) // 4:
        x, j = 0, 0
        while True:
            b = d[at]
            at += 1
            v = b & 0x7F
            x = ((-1 << 7) | v if v & 0x40 else v) if j == 0 else (x << 7) | v
            j += 1
            if not b & 0x80:
                break
        words.append(x & 0xFFFFFFFF)
    assert at <= size
    return {'cod': cod, 'dat': dat, 'hea': hea, 'stp': stp, 'cip': cip,
            'natives': [h for _, h in entries(natives, libraries)],
            'pubvars': entries(pubvars, tags),
            'code': words[:(dat - cod) // 4], 'data': words[(dat - cod) // 4:]}


MSG, FEMALE, WORK, WAIT = 0x9ADF1616, 0x59008363, 0x8E50F765, {0x15F86E59, 0xC8E7FB61}
RESULT = {5: 'halted', 6: 'budget', 7: 'yields', 8: 'memory', 9: 'bad'}


def s32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v >> 31 else v


class Fault(Exception):
    pass


def talk(sc, event, female=1, budget=200000, max_yields=500):
    """Pawn 3.3 (packed opcodes, position-independent jumps, HALT 12 = sleep)
    with the zone_talk native policy. Returns (result, steps, yields, natives,
    lines)."""
    code = sc['code']
    size = sc['stp'] - sc['dat']
    mem = bytearray(size)
    for k, w in enumerate(sc['data']):
        struct.pack_into('<I', mem, 4 * k, w)

    def rd(a):
        if a < 0 or a > size - 4:
            raise Fault
        return struct.unpack_from('<i', mem, a)[0]

    def wr(a, v):
        if a < 0 or a > size - 4:
            raise Fault
        struct.pack_into('<i', mem, a, s32(v))

    for address, h in sc['pubvars']:
        if h == 0xD7477C97:
            wr(address, event)
    pri = alt = frm = 0
    stk, hea = size, sc['hea'] - sc['dat']
    cip = sc['cip']
    state = {'yields': 0, 'natives': 0, 'lines': []}

    def push(v):
        nonlocal stk
        stk -= 4
        wr(stk, v)

    def pop():
        nonlocal stk
        v = rd(stk)
        stk += 4
        return v

    push(0)
    push(0)
    steps = 0
    try:
        while steps < budget:
            if cip < 0 or cip >= 4 * len(code) or cip % 4:
                return 'bad', steps, state['yields'], state['natives'], state['lines']
            w = code[cip // 4]
            op, p = w & 0xFFFF, s32(w) >> 16
            arg = lambda k: s32(code[cip // 4 + k])
            nxt = cip + 4
            if op in (0x78, 0xD2):                 # HALT, HALT_P
                v = arg(1) if op == 0x78 else p
                if op == 0x78:
                    nxt += 4
                if v != 12:
                    return 'halted', steps, state['yields'], state['natives'], state['lines']
                state['yields'] += 1
                if state['yields'] > max_yields:
                    return 'yields', steps, state['yields'], state['natives'], state['lines']
            elif op == 0x2E: push(frm); frm = stk                                  # PROC
            elif op == 0x30:                                                       # RETN
                frm = pop(); nxt = pop(); stk = s32(stk + rd(stk) + 4)
            elif op == 0x2F: frm = pop(); nxt = pop()                              # RET
            elif op == 0x31: push(cip + 8); nxt = cip + arg(1)                     # CALL
            elif op == 0x33: nxt = cip + arg(1)                                    # JUMP
            elif 0x35 <= op <= 0x40:                                               # Jcc
                u, v = pri & 0xFFFFFFFF, alt & 0xFFFFFFFF
                c = {0x35: pri == 0, 0x36: pri != 0, 0x37: pri == alt, 0x38: pri != alt, 0x39: u < v,
                     0x3A: u <= v, 0x3B: u > v, 0x3C: u >= v, 0x3D: pri < alt, 0x3E: pri <= alt,
                     0x3F: pri > alt, 0x40: pri >= alt}[op]
                nxt = cip + arg(1) if c else cip + 8
            elif op == 0x81:                                                       # SWITCH
                t = cip + arg(1)
                if code[t // 4] != 0x82:
                    return 'bad', steps, state['yields'], state['natives'], state['lines']
                count = code[t // 4 + 1]
                nxt = t + 8 + s32(code[t // 4 + 2]) - 4
                for k in range(count):
                    if s32(code[t // 4 + 3 + 2 * k]) == pri:
                        nxt = t + 16 + 8 * k + s32(code[t // 4 + 4 + 2 * k]) - 4
                        break
            elif op == 0x87:                                                       # SYSREQ_N
                index, nbytes = arg(1), arg(2)
                if not 0 <= index < len(sc['natives']):
                    return 'bad', steps, state['yields'], state['natives'], state['lines']
                push(nbytes)
                args = [rd(stk + 4 + 4 * k) for k in range(nbytes // 4)]
                h = sc['natives'][index]
                state['natives'] += 1
                if h == MSG:
                    if args and len(state['lines']) < 32:
                        state['lines'].append(args[0])
                    pri = 0
                else:
                    pri = female if h == FEMALE else 0 if h == WORK else 1 if h in WAIT else 0
                stk = s32(stk + nbytes + 4)
                nxt += 8
            elif op == 0xBC: push(p)                                               # PUSH_P_C
            elif op == 0xBD: push(rd(p))                                           # PUSH_P
            elif op == 0xBE: push(rd(frm + p))                                     # PUSH_P_S
            elif op == 0xD4: push(frm + p)                                         # PUSH_P_ADR
            elif op == 0x27: push(arg(1)); nxt += 4                                # PUSH_C
            elif op == 0x28: push(rd(arg(1))); nxt += 4                            # PUSH
            elif op == 0x29: push(rd(frm + arg(1))); nxt += 4                      # PUSH_S
            elif op == 0x85: push(frm + arg(1)); nxt += 4                          # PUSH_ADR
            elif 0x8A <= op <= 0x99:                                               # PUSHn_*
                n, kind = (op - 0x8A) // 4 + 2, (op - 0x8A) % 4
                for k in range(1, n + 1):
                    v = arg(k)
                    push(v if kind == 0 else rd(v) if kind == 1 else rd(frm + v) if kind == 2 else frm + v)
                nxt += 4 * n
            elif op == 0x24: push(pri)
            elif op == 0x25: push(alt)
            elif op == 0x2A: pri = pop()
            elif op == 0x2B: alt = pop()
            elif op == 0xBF: alt = stk; stk = s32(stk + p)                         # STACK_P
            elif op == 0x2C: alt = stk; stk = s32(stk + arg(1)); nxt += 4
            elif op == 0xC0: alt = hea; hea = s32(hea + p)
            elif op == 0x2D: alt = hea; hea = s32(hea + arg(1)); nxt += 4
            elif op == 0xA2: pri = rd(p)
            elif op == 0xA3: alt = rd(p)
            elif op == 0xA4: pri = rd(frm + p)
            elif op == 0xA5: alt = rd(frm + p)
            elif op == 0xA8: pri = rd(rd(frm + p))
            elif op == 0xA9: alt = rd(rd(frm + p))
            elif op == 0xA6: pri = rd(rd(p))
            elif op == 0xA7: alt = rd(rd(p))
            elif op == 0x01: pri = rd(arg(1)); nxt += 4
            elif op == 0x02: alt = rd(arg(1)); nxt += 4
            elif op == 0x03: pri = rd(frm + arg(1)); nxt += 4
            elif op == 0x04: alt = rd(frm + arg(1)); nxt += 4
            elif op == 0x9A: pri = rd(arg(1)); alt = rd(arg(2)); nxt += 8
            elif op == 0x9B: pri = rd(frm + arg(1)); alt = rd(frm + arg(2)); nxt += 8
            elif op == 0x9C: wr(arg(1), arg(2)); nxt += 8
            elif op == 0x9D: wr(frm + arg(1), arg(2)); nxt += 8
            elif op == 0x09: pri = rd(pri)                                         # LOAD_I
            elif op == 0x17: wr(alt, pri)                                          # STOR_I
            elif op == 0xAB: pri = p
            elif op == 0xAC: alt = p
            elif op == 0x0B: pri = arg(1); nxt += 4
            elif op == 0x0C: alt = arg(1); nxt += 4
            elif op == 0xAD: pri = frm + p
            elif op == 0xAE: alt = frm + p
            elif op == 0x0D: pri = frm + arg(1); nxt += 4
            elif op == 0x0E: alt = frm + arg(1); nxt += 4
            elif op == 0xAF: wr(p, pri)
            elif op == 0xB0: wr(p, alt)
            elif op == 0xB1: wr(frm + p, pri)
            elif op == 0xB2: wr(frm + p, alt)
            elif op == 0xB5: wr(rd(frm + p), pri)
            elif op == 0x0F: wr(arg(1), pri); nxt += 4
            elif op == 0x11: wr(frm + arg(1), pri); nxt += 4
            elif op == 0x59: pri = 0
            elif op == 0x5A: alt = 0
            elif op == 0xC7: wr(p, 0)
            elif op == 0xC8: wr(frm + p, 0)
            elif op == 0x21: pri = alt
            elif op == 0x22: alt = pri
            elif op == 0x23: pri, alt = alt, pri
            elif op == 0x4E: pri = s32(pri + alt)
            elif op == 0x4F: pri = s32(pri - alt)
            elif op == 0x50: pri = s32(alt - pri)
            elif op == 0x51: pri = s32(pri & alt)
            elif op == 0x52: pri = s32(pri | alt)
            elif op == 0x53: pri = s32(pri ^ alt)
            elif op == 0x54: pri = int(pri == 0)
            elif op == 0x55: pri = s32(-pri)
            elif op == 0x56: pri = s32(~pri)
            elif op == 0x48 or op == 0x4B: pri = s32(pri * alt)
            elif op in (0x49, 0x4A):
                a, b = (pri, alt) if op == 0x49 else (alt, pri)
                if b == 0:
                    return 'bad', steps, state['yields'], state['natives'], state['lines']
                q = a // b  # Python floors, as Pawn's SDIV
                pri, alt = s32(q), s32(a - q * b)
            elif op == 0xC5: pri = s32(pri + p)
            elif op == 0xC6: pri = s32(pri * p)
            elif op == 0x57: pri = s32(pri + arg(1)); nxt += 4
            elif op == 0x5F: pri = int(pri == alt)
            elif op == 0x60: pri = int(pri != alt)
            elif op == 0x65: pri = int(pri < alt)
            elif op == 0x66: pri = int(pri <= alt)
            elif op == 0x67: pri = int(pri > alt)
            elif op == 0x68: pri = int(pri >= alt)
            elif op == 0xC9: pri = int(pri == p)
            elif op == 0xCA: pri = int(alt == p)
            elif op == 0x69: pri = int(pri == arg(1)); nxt += 4
            elif op == 0x6B: pri = s32(pri + 1)
            elif op == 0x70: pri = s32(pri - 1)
            elif op == 0xCB: wr(p, rd(p) + 1)
            elif op == 0xCC: wr(frm + p, rd(frm + p) + 1)
            elif op == 0xCD: wr(p, rd(p) - 1)
            elif op == 0xCE: wr(frm + p, rd(frm + p) - 1)
            elif op == 0xB9: pri = s32((pri << (p & 31)) + alt)                    # IDXADDR_P_B
            elif op == 0xB8: pri = rd(s32((pri << (p & 31)) + alt))                # LIDX_P_B
            elif op == 0x1B: pri = s32(pri * 4 + alt)
            elif op == 0x19: pri = rd(s32(pri * 4 + alt))
            elif op == 0xC1: pri = s32(pri << (p & 31))
            elif op == 0x41: pri = s32(pri << (alt & 31))
            elif op == 0x42: pri = s32((pri & 0xFFFFFFFF) >> (alt & 31))
            elif op == 0x43: pri = pri >> (alt & 31)
            elif op in (0x89, 0x86): pass                                          # BREAK, NOP
            elif op == 0x83: v = rd(stk); wr(stk, pri); pri = v
            elif op in (0xD1, 0x77):                                               # FILL
                t = p if op == 0xD1 else arg(1)
                if op == 0x77:
                    nxt += 4
                for k in range(0, t - 3, 4):
                    wr(alt + k, pri)
            elif op in (0xCF, 0x75):                                               # MOVS
                t = p if op == 0xCF else arg(1)
                if op == 0x75:
                    nxt += 4
                if pri < 0 or alt < 0 or pri + t > size or alt + t > size:
                    raise Fault
                mem[alt:alt + t] = mem[pri:pri + t]
            else:
                return 'bad', steps, state['yields'], state['natives'], state['lines']
            if stk < hea:
                raise Fault
            steps += 1  # counted once complete, as the native machine does
            cip = nxt
        return 'budget', steps, state['yields'], state['natives'], state['lines']
    except Fault:
        return 'memory', steps, state['yields'], state['natives'], state['lines']


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--romfs', required=True, type=Path)
    parser.add_argument('--bridge', required=True, type=Path)
    parser.add_argument('--viewer', type=Path)
    parser.add_argument('--output-dir', required=True, type=Path)
    args = parser.parse_args()
    out = args.output_dir
    for sub in ('text', 'scripts'):
        (out / sub).mkdir(parents=True, exist_ok=True)
    report = {'archive_sha256': {a: hashlib.sha256((args.romfs / a).read_bytes()).hexdigest()
                                 for a in ('a/0/8/2', 'a/0/1/2')},
              'verifier_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'bridge_source_sha256': hashlib.sha256((HERE / 'dump_talk.c').read_bytes()).hexdigest()}
    # Text.
    texts = {}
    files = garc_count(args.romfs / 'a/0/8/2')
    rendered_lines = unrenderable = 0
    for number in range(files):
        data = member(args.romfs, 'a/0/8/2', number)
        path = out / 'text' / f'{number}.bin'
        path.write_bytes(data)
        lines = text_lines(data)
        mine = [render(l) for l in lines]
        native = json.loads(subprocess.run([str(args.bridge), '--text', str(path)], capture_output=True,
                                           text=True, check=True, timeout=120).stdout)
        assert native['result'] == 0 and native['lines'] == mine, number
        texts[number] = lines
        rendered_lines += sum(m is not None for m in mine)
        unrenderable += sum(m is None for m in mine)
    report['text'] = {'files': files, 'lines': rendered_lines + unrenderable, 'unrenderable': unrenderable,
                      'all_lines_match': True}
    # Scripts and talks.
    results, with_lines, line_total = {}, 0, 0
    zones = garc_count(args.romfs / 'a/0/1/2') - 1
    for number in range(zones):
        parts = walk.sections(member(args.romfs, 'a/0/1/2', number), b'ZO')
        npcs = parse_entities(parts[1])['npcs']
        if not npcs:
            continue
        text_id = struct.unpack_from('<H', parts[0], 6)[0]
        sc = decode_script(parts[2])
        path = out / 'scripts' / f'{number}.bin'
        path.write_bytes(parts[2])
        events = [n[5] for n in npcs]
        native = [json.loads(l) for l in subprocess.run([str(args.bridge), str(path), *map(str, events)],
                                                        capture_output=True, text=True, check=True,
                                                        timeout=600).stdout.splitlines()]
        for event, n in zip(events, native):
            result, steps, yields, natives, lines = talk(sc, event)
            assert RESULT[n['result']] == result and n['steps'] == steps and n['yields'] == yields, (number, event)
            assert n['natives'] == natives and n['lines'] == lines, (number, event)
            for line in lines:
                assert 0 <= line < len(texts[text_id]), (number, event, line)
            results[result] = results.get(result, 0) + 1
            with_lines += bool(lines)
            line_total += len(lines)
    report['talks'] = {'npcs': sum(results.values()), 'results': results, 'npcs_with_lines': with_lines,
                       'message_lines': line_total, 'all_lines_in_zone_text': True, 'all_runs_match': True}
    if args.viewer:
        inspect = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '4', '--inspect'],
                                 capture_output=True, text=True, check=True, timeout=120).stdout
        viewer = {'inspect': [l for l in inspect.split('\n') if l.startswith(('dialogue ', 'npc_dialogue '))]}
        capture = out / 'talk.bmp'
        if capture.exists():
            capture.unlink()
        result = subprocess.run([str(args.viewer), '--romfs', str(args.romfs), '--zone', '4', '--arrive', '1',
                                 '--play', 'RRRRUUUURRUU.', '--software', '--frames', '330', '--capture-frame',
                                 str(capture), '--capture-at', '329'], capture_output=True, text=True, check=True,
                                timeout=900, env={'SDL_VIDEODRIVER': 'dummy', 'PATH': '/usr/bin:/bin'})
        viewer['runtime'] = [l for l in result.stdout.split('\n') if l.startswith(('talk ', 'message ', 'dialogue zone=4 script='))]
        viewer['capture_sha256'] = hashlib.sha256(capture.read_bytes()).hexdigest()
        expected = [render(l, 'Player').split('\f') for l in [texts[53][1]]][0]
        pages = [l.split(' text="', 1)[1][:-1].replace('\\n', '\n') for l in viewer['runtime'] if l.startswith('message ')]
        assert pages == expected, (pages, expected)
        report['viewer'] = viewer
    (out / 'reference-comparison.json').write_text(json.dumps(report, indent=1, ensure_ascii=False) + '\n')
    print(json.dumps(report, indent=1, ensure_ascii=False)[:3000])


if __name__ == '__main__':
    main()
