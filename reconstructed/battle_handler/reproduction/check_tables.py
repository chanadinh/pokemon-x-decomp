#!/usr/bin/env python3
"""Register every original ability, item, move and field handler set through the reconstruction.

Reads DllBattle.cro and the relocation audit from scripts/prepare_cro_analysis.py,
decodes each lookup-table row and its four-instruction table function
(mov r1, #word or ldr r1, [pc, #k]; str r1, [r0]; ldr r0, [pc, #k]; bx lr), checks every handler entry
table, then builds the reconstruction as a shared library and registers each
row with the original bytes served at their static addresses. No original data
is retained; the output is a summary.
"""
import argparse
import collections
import ctypes
import hashlib
import json
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

TABLES = {'ability': (0x108d510c, 0xb6, 'B'), 'item': (0x108d382c, 0xb6, 'I'),
          'move': (0x108d3ddc, 0x128, 'H'), 'field': (0x108d6810, 5, 'B')}
MOV_R1, STR_R1, LDR_R0, BX_LR = 0xe3a01000, 0xe5801000, 0xe59f0000, 0xe12fff1e
forms = collections.Counter()


def require(condition, message):
    if not condition:
        raise ValueError(message)


class Module:
    def __init__(self, cro, manifest, relocations):
        self.cro = cro
        self.segments = [(s['analysis_address'], s['size'], s['offset'], s['index'])
                         for s in manifest['segments'] if s['size'] and s['index'] in (0, 1, 2)]
        self.relocated = {r['address']: r['value'] for r in relocations}

    def segment(self, address):
        for start, size, offset, index in self.segments:
            if start <= address < start + size:
                return offset + address - start, index
        return None, None

    def byte(self, address):
        offset, _ = self.segment(address)
        require(offset is not None, 'Address %#x outside the module' % address)
        word = address & ~3
        if word in self.relocated:
            return struct.pack('<I', self.relocated[word])[address - word]
        return self.cro[offset]

    def word(self, address):
        return int.from_bytes(bytes(self.byte(address + i) for i in range(4)), 'little')


def decode_function(module, address):
    """The table function's count word and entries pointer."""
    _, index = module.segment(address)
    require(index == 0, 'Table function %#x is not in the text segment' % address)
    w = [module.word(address + 4 * i) for i in range(4)]
    require(w[1] == STR_R1 and (w[2] & 0xfffff000) == LDR_R0 and w[3] == BX_LR, 'Unexpected table function %#x' % address)
    if (w[0] & 0xfffff000) == MOV_R1:
        rotate, imm = (w[0] >> 8) & 0xf, w[0] & 0xff
        count_word = ((imm >> (2 * rotate)) | (imm << (32 - 2 * rotate))) & 0xffffffff if rotate else imm
        forms['mov'] += 1
    else:
        # ldr r1, [pc, #k]: the count word is a literal (pc reads 8 bytes ahead).
        require((w[0] & 0xfffff000) == 0xe59f1000, 'Unexpected count load in %#x' % address)
        literal = address + 8 + (w[0] & 0xfff)
        require(literal not in module.relocated, 'Relocated count word in %#x' % address)
        count_word = module.word(literal)
        forms['ldr'] += 1
    entries = address + 8 + 8 + (w[2] & 0xfff)
    require(entries in module.relocated, 'Entries literal of %#x is not relocated' % address)
    return count_word, module.relocated[entries]


def build_library(root, out):
    sources = [root / 'reconstructed/battle_handler/battle_handler.c']
    subprocess.run(['cc', '-std=c11', '-O2', '-shared', '-fPIC', '-o', str(out)] + [str(s) for s in sources], check=True)
    return ctypes.CDLL(str(out))


U32 = ctypes.c_uint32
LOAD8 = ctypes.CFUNCTYPE(ctypes.c_uint8, ctypes.c_void_p, U32)
LOAD16 = ctypes.CFUNCTYPE(ctypes.c_uint16, ctypes.c_void_p, U32)
LOAD32 = ctypes.CFUNCTYPE(U32, ctypes.c_void_p, U32)
STORE8 = ctypes.CFUNCTYPE(None, ctypes.c_void_p, U32, ctypes.c_uint8)
STORE16 = ctypes.CFUNCTYPE(None, ctypes.c_void_p, U32, ctypes.c_uint16)
STORE32 = ctypes.CFUNCTYPE(None, ctypes.c_void_p, U32, U32)
ABORT = ctypes.CFUNCTYPE(None, ctypes.c_void_p)
VALUE = ctypes.CFUNCTYPE(U32, ctypes.c_void_p, U32, U32)
ENTRIES = ctypes.CFUNCTYPE(U32, ctypes.c_void_p, U32, U32)


class Memory(ctypes.Structure):
    _fields_ = [('user', ctypes.c_void_p), ('parameters', ctypes.c_void_p), ('load8', LOAD8), ('load16', LOAD16),
                ('load32', LOAD32), ('store8', STORE8), ('store16', STORE16), ('store32', STORE32)]


class Api(ctypes.Structure):
    _fields_ = [('memory', ctypes.POINTER(Memory)), ('user', ctypes.c_void_p), ('abort', ABORT),
                ('pokemon_value', VALUE), ('raw_value', VALUE), ('entries', ENTRIES)] + \
               [(n, U32) for n in ('battle', 'pool', 'ability_table', 'item_table', 'move_table', 'field_table',
                                   'count_scratch')]


RAM, BATTLE, POOL, NODES, POKEMON, SCRATCH = 0x20000000, 0x20000000, 0x20000100, 0x20001000, 0x20004000, 0x20005000


def run(root, cro_path):
    cro = cro_path.read_bytes()
    manifest = next(m for m in json.loads((root / 'outputs/cro-prepared/manifest.json').read_text())['modules']
                    if m['name'] == 'DllBattle')
    relocations = [r for r in json.loads((root / 'outputs/cro-prepared/relocations.json').read_text())['applied']
                   if r['module'] == 'DllBattle']
    for r in relocations:
        require(r['value'] == (r['symbol_address'] + r['addend']) & 0xffffffff, 'Relocation is not S+A')
    module = Module(cro, manifest, relocations)

    rows, functions, entry_tables, handlers, events = {}, {}, {}, set(), collections.Counter()
    for name, (base, count, key) in TABLES.items():
        rows[name] = []
        size = struct.calcsize('<' + key)
        for i in range(count):
            at = base + 8 * i
            ident = int.from_bytes(bytes(module.byte(at + k) for k in range(size)), 'little')
            if at + 4 not in module.relocated:
                require(name == 'item' and i == count - 1 and ident == 0, 'Row without a function: %s %d' % (name, i))
                continue
            function = module.relocated[at + 4]
            if function not in functions:
                functions[function] = decode_function(module, function)
            word, table = functions[function]
            n = word & 0xffff
            if table not in entry_tables:
                entries = []
                for k in range(n):
                    e = table + 8 * k
                    require(e + 4 in module.relocated, 'Handler entry without a function')
                    handler = module.relocated[e + 4]
                    require(module.segment(handler)[1] == 0, 'Handler outside the text segment')
                    entries.append((module.byte(e), handler))
                entry_tables[table] = entries
            for event, handler in entry_tables[table]:
                handlers.add(handler)
                events[event] += 1
            rows[name].append((ident, function, word, table))
        require(len({r[0] for r in rows[name]}) == len(rows[name]), 'Duplicate ids in the %s table' % name)

    with tempfile.TemporaryDirectory() as tmp:
        lib = build_library(root, Path(tmp) / 'libbattle_handler.so')
        ram = bytearray(0x6000)

        def location(address):
            return address - RAM if RAM <= address < RAM + len(ram) else None

        def load8(_, a):
            o = location(a)
            return ram[o] if o is not None else module.byte(a)

        def load16(u, a):
            return load8(u, a) | load8(u, a + 1) << 8

        def load32(u, a):
            return load16(u, a) | load16(u, a + 2) << 16

        def store8(_, a, v):
            o = location(a)
            require(o is not None, 'Write outside RAM at %#x' % a)
            ram[o] = v

        def store16(u, a, v):
            store8(u, a, v & 0xff)
            store8(u, a + 1, v >> 8)

        def store32(u, a, v):
            store16(u, a, v & 0xffff)
            store16(u, a + 2, v >> 16)

        state = {'aborts': 0, 'ability': 0, 'speed': 0}

        def abort(_):
            state['aborts'] += 1

        def pokemon_value(_, p, k):
            require(p == POKEMON and k == 0x10, 'Unexpected value request')
            return state['ability']

        def raw_value(_, p, k):
            require(p == POKEMON and k == 0xc, 'Unexpected raw value request')
            return state['speed']

        def entries(_, function, out):
            require(out == SCRATCH and function in functions, 'Unknown table function %#x' % function)
            word, table = functions[function]
            store32(None, out, word)
            return table

        callbacks = [LOAD8(load8), LOAD16(load16), LOAD32(load32), STORE8(store8), STORE16(store16), STORE32(store32)]
        memory = Memory(None, None, *callbacks)
        hooks = [ABORT(abort), VALUE(pokemon_value), VALUE(raw_value), ENTRIES(entries)]
        api = Api(ctypes.pointer(memory), None, *hooks, BATTLE, POOL,
                  TABLES['ability'][0], TABLES['item'][0], TABLES['move'][0], TABLES['field'][0], SCRATCH)
        for f, types in (('px_battle_register_ability', [U32]), ('px_battle_register_item', [U32]),
                         ('px_battle_register_move', [U32, U32, U32]),
                         ('px_battle_register_field', [U32, U32, U32, U32, U32])):
            getattr(lib, f).argtypes = types + [ctypes.POINTER(Api)]
            getattr(lib, f).restype = U32

        def fresh():
            ram[:] = bytes(len(ram))
            for i in range(0x9d):
                store32(None, POOL + 4 * i, NODES + 0x40 * i)
            store8(None, POKEMON + 0x19, 3)
            state['aborts'] = 0

        def check(handler, owner, ident, priority, sub, value, word, table):
            require(handler == NODES and state['aborts'] == 0, 'Registration failed')
            require(load8(None, handler + 0x10) == owner and load16(None, handler + 0x38) == ident, 'Owner or id differs')
            require(load32(None, handler + 0x14) == (sub | ((7 - priority) & 0xff) << 24), 'Sort key differs')
            require(load32(None, handler + 8) == table and load8(None, handler + 0x1a) == word & 0xff, 'Entries differ')
            require(load8(None, handler + 0x3a) == value and load32(None, BATTLE + 0x60) == handler, 'Value or list differs')

        priorities = collections.Counter()
        for ident, function, word, table in rows['ability']:
            fresh()
            state['ability'], state['speed'] = ident, 0x1234
            priority = (word >> 16 or 5) & 0xff
            priorities['ability', priority] += 1
            check(lib.px_battle_register_ability(POKEMON, ctypes.byref(api)), 4, ident, priority, 0x1234, 3, word, table)
        for ident, function, word, table in rows['item']:
            fresh()
            store16(None, POKEMON + 0x12, ident)
            state['speed'] = 0x77
            check(lib.px_battle_register_item(POKEMON, ctypes.byref(api)), 5, ident, 6, 0x77, 3, word, table)
        for ident, function, word, table in rows['move']:
            fresh()
            require(lib.px_battle_register_move(POKEMON, ident, 0x55, ctypes.byref(api)) == 1, 'Move not registered')
            check(NODES, 0, ident, 0, 0x55, 3, word, table)
        for ident, function, word, table in rows['field']:
            fresh()
            check(lib.px_battle_register_field(ident, 7, 0xfeed, 0, 0, ctypes.byref(api)), 1, ident, 1, 0, 7, word, table)
            require(load32(None, NODES + 0x1c + 24) == 0xfeed, 'Field work word differs')

    return {
        'module_sha256': hashlib.sha256(cro).hexdigest(),
        'rows': {name: len(r) for name, r in rows.items()},
        'item_terminator_rows': 1,
        'table_functions': len(functions),
        'table_function_forms': dict(sorted(forms.items())),
        'entry_tables': len(entry_tables),
        'handler_entries': sum(len(e) for e in entry_tables.values()),
        'distinct_handler_functions': len(handlers),
        'distinct_events': len(events),
        'ability_priorities': {str(p): n for (kind, p), n in sorted(priorities.items())},
        'registrations_checked': sum(len(r) for r in rows.values()),
        'all_match': True,
    }


def main():
    root = Path(__file__).resolve().parents[3]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cro', type=Path, default=root / 'inputs/pokemon-x-modules-a035d9775a1f/modules/DllBattle.cro')
    args = parser.parse_args()
    try:
        record = run(root, args.cro)
    except (OSError, ValueError, KeyError, subprocess.CalledProcessError) as exc:
        parser.exit(1, f'error: {exc}\n')
    json.dump(record, sys.stdout, indent=2)
    print()


if __name__ == '__main__':
    main()
