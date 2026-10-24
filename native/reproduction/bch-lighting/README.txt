Native BCH directional fragment lighting, 2026-10-24

This milestone finds the selected field's lights, decodes them, and executes
the part of PICA fragment lighting the field's materials use: directional
diffuse and ambient terms, per fragment, with interpolated normals. Fragment
primary was white in ../bch-combiners and is now lit. Tracked files contain
independently authored source, synthetic tests and metadata only. No payload,
pixel array or capture is included; the two documentation PNGs are recorded in
docs/images/README.md.

Native preview

make -C native all test test-runtime
native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured --combiners \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11 \
  --environment-archive a/0/0/3 --environment-entry 137 --environment-section 0 \
  --lighting day                       # or night

The environment options require --combiners. Without them the combiner
preview is unchanged (its recorded capture reproduces byte-for-byte, now also
from the real archives). --inspect prints the environment, each mesh's normal
source and each material's light setup. The model viewer's --section now also
accepts BM minipacks (same layout as AD/GR: magic, u16 count, u32 offsets).

Where the lights are

The field model carries no lights, LUTs or shaders, and neither do the area
packs. A scan of every GARC member in the RomFS (after LZ and nested
minipacks) found lights and scenes only in a/0/0/3 entry 137, a BM pack whose
section 0 is a BCH with 16 lights, one scene "scene_environment_field" with 12
light sets (20 entries) and one fog. Section 1 holds a 10-light variant with 6
sets. That this entry is the one the game binds to field 165 is not traced: it
is the only candidate, and every material's light-set index (1 for the window
chip, 0 for the others) resolves in it.

Decoding (assets/bch_scene.h, implemented in bch_model.c)

Lights follow the pinned SPICA H3DLight/H3DFragmentLight order: name, scale,
rotation, translation, type, enabled, flags, LUT config, metadata, then
ambient, diffuse, specular0, specular1 (RGBA8), direction, two LUT sampler
pointers and attenuation start/end. Types 9/10/11 (fragment directional,
point, spot) and 2 (ambient) are accepted; flags beyond two-sided diffuse (bit
0) and distance attenuation (bit 1) are rejected. Scene records give name,
then (pointer, count) for cameras, light sets and fogs. Each light-set entry
in this file is 12 bytes: set index, pointer to an array of light-name string
pointers, count. Ohana3DS c50635e reads 8-byte (slot, name) references
instead; the verifier shows that reading is inconsistent with this file's
relocations (the second word is a contents pointer, not a string), so the
12-byte grouping is an observation from the data, recorded as such.

The model reader now returns normals (stored semantic 1 after its uniform
scale, or fixed float24, as for colors) and each material's lighting
parameters (flags, fragment flags, light-set and fog index, emission/ambient/
diffuse/specular colors, color scale, LUT sampler pointers, Fresnel selector,
bump mode). Field 165: seven meshes store normals, five use fixed normals; all
twelve materials enable fragment lighting with no LUTs, Fresnel or bump.

Conventions from the original code (code.bin sha256 0f6d3e1d...797a)

- Light upload 0x2df3f0: for runtime light kind 0 it builds (direction, w=0)
  from the light object at +0x17c, transforms it by the view matrix
  (0x2e0050), negates all four components and writes it with 0x2e2dc0 to the
  light's position registers 0x144/0x145 (header 0x801f0144 + 16 * slot,
  truncating float16). It sets the directional bit when the transformed w is
  0. So the hardware light vector is -direction in view space.
- Color loop 0x3030a8: each light color register (0x140..0x143: specular0,
  specular1, diffuse, ambient; header 0x803f0140 + 16 * slot) is
  ((2*light + 1) * (2*material + 1)) >> 10 per channel, light colors at runtime
  +0xfc..+0x108 against material +0xb4..+0xc0. Global ambient is material
  emission (+0xb0), plus ambient-light * material ambient only when an ambient
  light object is bound (0x302fcc); with none, emission alone (0x30306c).
- Not traced: how +0x17c is derived from the resource direction. All sixteen
  lights have identity rotation and unit scale, so any rigid world transform
  leaves the direction unchanged.
The verifier checks 44 instruction and literal words at these addresses.

Lighting (render/light.c) and viewer policy

px_light_primary follows Azahar 1b5fd009 ComputeFragmentsColors in float32
for this subset: light vector normalized, dot = max(N.L, 0) (|N.L| if
two-sided), diffuse_sum = sum(diffuse * dot + ambient) + global ambient, each
channel clamped and truncated to u8, alpha 255. Lights are register values.
N.L is evaluated in model space (identity model transform): it equals the
view-space value except for the float16 truncation of the view-space light
vector, which is bounded below. Normals are interpolated per fragment through a
new rasterizer varying (px_raster_draw_varying) and renormalized; hardware
interpolates a normal quaternion from the (unexecuted) vertex shader, so this
is an approximation.

Per material, the enabled lights of its light set are used: directional ones
execute; point/spot lights are not executed, and the sum of their ambient +
diffuse registers per channel is reported as a bound (valid if attenuation and
dot are within 0..1). Light set 0 enables both light_sun and light_night, so
the game must select one at run time; that selection is not traced, and
--lighting day (default) omits light_night, night omits light_sun.

Field 165 result

Window chip: light_window only, ambient (255,229,128), diffuse 0: a constant
warm fragment primary. Other materials, day: light_sun, ambient (199,208,226),
diffuse (79,104,98), vector (0,0.9968,-0.0797); floors saturate, walls get the
ambient alone. Night: light_night, ambient (61,71,107), diffuse (89,158,112).
Point/spot bound 0,3,0 (light_point_b diffuse; light_point_a is black). The
viewer evaluates every raise of fragment primary within the bound: the final
color could change by at most 4 levels (point_bound_max_delta), in fragments of
six materials by day and nine by night; no degenerate normals.

Independent comparison

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror native/assets/bch_model.c \
  native/render/light.c native/reproduction/bch-lighting/dump_lighting.c -lm \
  -o native/build/dump_lighting
python3 -I -B native/reproduction/bch-lighting/verify_lighting.py \
  --field /path/to/field165.bch --environment /path/to/a003-137-section0.bch \
  --code /path/to/code.bin --bridge native/build/dump_lighting \
  --viewer native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --spica /path/to/SPICA --ohana /path/to/Ohana3DS-Rebirth --azahar /path/to/azahar \
  --output-dir outputs/new-lighting-check

The verifier parses lights, scene sets, material parameters and normals itself
(reusing only the section/relocation resolver of ../bch-textured and the
command-packet walker of ../bch-combiners) and writes the lighting oracle from
the pinned Azahar source. Recorded result (reference-comparison.json):
- 16 lights (every field), 12 sets, 20 set entries, the scene name and counts
  match; the Ohana 8-byte reading is rejected by the relocations.
- 12 materials' lighting parameters and 7,068 normal components (2,356
  vertices) match bit for bit.
- 44 code.bin words match; all 65,536 register products match.
- 20,000 random lighting setups (0..8 lights, 10-bit colors, two-sided,
  2% zero normals; 394 invalid inputs agree) and 970 field setups x normals
  (day and night) match byte for byte.
- Float16 bound: 200 random view rotations x every distinct field normal,
  day and night (194,000 comparisons): the game's truncating float16 light
  vector changes fragment primary by at most 1 level.
- Viewer (real archive paths, day and night): every light_setup line equals
  the oracle's; captures (3 frames, input self-test) sha256
  day 1dc1e452...243a, night 640c936a...00fe.

Tests

assets/test_bch_scene.c: a hand-built environment (three lights, two sets in a
different order) with every field checked, and rejection of hemisphere/vertex
types, enabled 2, unknown flags, non-finite direction/attenuation, tree/name
mismatch, duplicate names and set indices, unresolved names, overrunning arrays,
limits, duplicate relocations and every truncation. assets/test_bch_model.c:
fixed and stored (int8, scaled) normals and material lighting parameters.
render/test_light.c: register products (all l with m = 255) and hand-checked
float32 results, including truncation, clamping, two-sided and invalid input.
render/test_raster.c: varyings follow vertices through the winding swap.
tests/test_bch_lighting_viewer.py: day/night pixels equal the separately
computed bytes, inspect lines, the bound report, and failure before video for
disabled lighting, an absent light set, a missing normal, ambient and
hemisphere lights, and the option contract.

Limits

Lighting matches an emulator's documented behavior, not measured hardware or a
game capture. Not executed: the vertex shader (normal quaternion, primary
color), point/spot lights (bounded), the runtime day/night selection, specular
and the fragment secondary (shown irrelevant to this field's chains in
../bch-combiners), fog, projected textures, culling and camera. The binding of
a/0/0/3 entry 137 to this field is inferred, not traced.
