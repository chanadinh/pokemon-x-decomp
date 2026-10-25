"""The Pokémon models' own vertex shaders against the CPU path (a/0/0/7).

For every model (entry 4 + 8m, PC section 0) runs native/build/shader_check:
each mesh's material names its program ("E@Name" -> shader resource -> DVLB
entry E); uniforms come from the DVLE constants and the material, mesh and
submesh command streams, the bone palette and identity matrices are
supplied, and every drawn vertex runs in the software PICA interpreter in
the rest pose. Its o0 must match render/skin.h's posed position and its
o4.xy the decoded UV through the material's TexMtx0 rows. Then the same for
seven battle models posed by their idle, physical-attack and damage motions
(first, middle and last frames).

Tolerances: position error relative, |shader - cpu| / (1 + |cpu|), at most
1e-5 (the shader works in float32, the CPU path in double); o0.w exactly
1; UV error at most 1e-5. Meshes drawn with the flame program 0@PokeFire are
reported but not required to match (it reads engine-supplied UserMatrix0..2
and PressDir). Writes reference-comparison.json.
Run with python3 -I. Output may contain local asset data.
"""
import argparse
import importlib.util
import json
from pathlib import Path
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, HERE / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


shaders = load('shaders', '../platform/verify_shaders.py')
walk = shaders.walk


ANIMATED = (18, 21, 354, 864, 867, 869, 872)  # the models of ../bch-battle-motion's sample


def fields(line):
    return dict(word.split('=', 1) for word in line.split() if '=' in word)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--romfs', type=Path, required=True)
    ap.add_argument('--check', type=Path, required=True)
    ap.add_argument('--models', type=int, default=981)
    args = ap.parse_args()
    garc = shaders.Garc(args.romfs / 'a/0/0/7')
    totals = {'models': 0, 'empty_models': [], 'meshes': 0, 'vertices': 0, 'failures': []}
    worst = {'position_error': 0.0, 'w_error': 0.0, 'uv_error': 0.0, 'quaternion_from_identity': 0.0}
    programs, bools = {}, {}
    sample = None
    outliers = []
    animated = {'runs': [], 'vertex_runs': 0, 'position_error': 0.0, 'w_error': 0.0, 'uv_error': 0.0}
    with tempfile.TemporaryDirectory() as scratch:
        path = Path(scratch) / 'model.bch'
        for m in range(args.models):
            bch = walk.sections(garc.member(4 + 8 * m), b'PC')[0]
            if not bch:
                totals['empty_models'].append(m)
                continue
            path.write_bytes(bch)
            run = subprocess.run([str(args.check), str(path)], capture_output=True, text=True)
            lines = run.stdout.splitlines()
            if run.returncode or not lines or not lines[-1].startswith('summary'):
                totals['failures'].append({'model': m, 'output': lines[-3:]})
                continue
            totals['models'] += 1
            summary = fields(lines[-1])
            totals['meshes'] += int(summary['meshes'])
            totals['vertices'] += int(summary['vertices'])
            for line in lines[:-1]:
                f = fields(line)
                if float(f['position_error']) > 1e-5 or float(f['uv_error']) > 1e-5 or f['w_error'] != '0':
                    outliers.append({'model': m, **{k: f[k] for k in ('mesh', 'name', 'material', 'shader', 'bools',
                                     'vertices', 'position_error', 'w_error', 'uv_error')}})
                key = f['shader']
                for name in worst:
                    if key != '0@PokeFire':
                        worst[name] = max(worst[name], float(f[name]))
                programs[key] = programs.get(key, 0) + 1
                bools[f['bools']] = bools.get(f['bools'], 0) + 1
            if m == 864:
                sample = lines
        # Animated poses for the battle sample of ../bch-battle-motion: the idle,
        # physical attack and damage motions (slots 1, 4, 17), each at its first,
        # middle and last frame.
        motion_path = Path(scratch) / 'motion.bin'
        for m in ANIMATED:
            path.write_bytes(walk.sections(garc.member(4 + 8 * m), b'PC')[0])
            motion_path.write_bytes(walk.sections(garc.member(4 + 8 * m + 5), b'PB')[0])
            for slot in (1, 4, 17):
                run = subprocess.run([str(args.check), str(path), '--motion', str(motion_path), '--slot', str(slot)],
                                     capture_output=True, text=True)
                lines = run.stdout.splitlines()
                if run.returncode or not lines or not lines[-1].startswith('summary'):
                    totals['failures'].append({'model': m, 'slot': slot, 'output': lines[-3:]})
                    continue
                for line in lines[:-1]:
                    f = fields(line)
                    if f['pose'] == '0' or f['shader'] == '0@PokeFire':
                        continue
                    animated['vertex_runs'] += int(f['vertices'])
                    for name in ('position_error', 'uv_error'):
                        animated[name] = max(animated[name], float(f[name]))
                    animated['w_error'] = max(animated['w_error'], float(f['w_error']))
                animated['runs'].append(f'{m}:{slot}')
    # The flame program (0@PokeFire) places vertices with UserMatrix0..2 (c22..c24) and PressDir
    # (c85), which the engine supplies per draw and which are not traced, so its meshes are listed,
    # not compared. Every other mesh must match.
    compared = [o for o in outliers if o['shader'] != '0@PokeFire']
    ok = (not totals['failures'] and not compared and animated['position_error'] <= 1e-5 and
          animated['w_error'] == 0 and animated['uv_error'] <= 1e-5)
    result = {**totals, 'worst_compared': worst, 'animated': animated, 'meshes_by_program': dict(sorted(programs.items())),
              'distinct_bool_words': len(bools), 'outliers': outliers,
              'outliers_not_comparable': len(outliers) - len(compared), 'froakie_model_864': sample, 'passed': ok}
    (HERE / 'reference-comparison.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({k: v for k, v in result.items() if k not in ('froakie_model_864', 'outliers')}, indent=2))
    print(len(outliers), 'outlying meshes')
    assert ok


if __name__ == '__main__':
    main()
