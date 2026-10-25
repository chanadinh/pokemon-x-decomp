The PICA fragment lighting unit for the battle models, 2026-10-25

With --vertex-shader the battle models' fragment lighting is now executed as
the PICA's lighting unit computes it (render/frag_light.h), from the outputs
of the models' own vertex programs (../vertex-shader): the interpolated
normal quaternion (o1) and view vector (o2), the bump texture, and the
materials' lighting LUTs. Before, the viewer lit them with interpolated
normals and the scene's directional lights only, and their secondary color
(the specular sum) was always zero. Off by default; without the option every
frame is unchanged. Tracked files contain independently authored source,
synthetic tests and metadata only.

What the Pokémon materials ask for

verify_fragment_lighting.py reads every Pokémon model's LUT sets and
material lighting state independently and compares them with the decoder
(977 models, 6,376 LUT samplers, 5,877 materials: all identical). 5,777
materials enable the reflection LUTs (fragment flags 0x10) and nothing else;
100 enable no LUT. Their LUT inputs are signed (absolute disabled) N.V for
the red, green and blue reflection tables (5,509 materials), N.L for 265
(0x3330000), N.L for red alone for 3. 5,509 materials bump-map with a normal
map in texture unit 2 (bump mode 1), 368 do not; none renormalizes the bump
normal.

The programs supply an identity quaternion and, as the view vector, the light
direction in the vertex's tangent frame. So on the hardware N is the
normal-map texel and N.V is the bumped N.L in tangent space: the three
reflection tables turn it into three ramps (a specular curve, a per-Pokémon
"Hta" table and a linear table, e.g. Froakie's Default_SpcLta_6D,
pm0723_00_TattuHta, LinerTable) in the secondary color's R, G and B, which
the combiners then use. Fragment primary sees a normal facing the viewer.

render/frag_light.h

The unit for directional lights after Azahar 93f8939a sw_lighting.cpp
ComputeFragmentsColors: normal map and quaternion rotation, half vector,
LUT inputs N.H, V.H, N.V, L.N, absolute and signed LUT indexing with the
entries' 12-bit value and signed 11-bit difference, input scales, the
specular0/specular1/reflection terms, Fresnel on the last light, clamp
highlights, geometric factors, and the clamped, truncated sums. Not
implemented: distance and spot attenuation, shadows, tangent maps, the
SP/CP inputs (0 here), positional lights. test_frag_light.c checks LUT
indexing and interpolation, register decoding, diffuse, reflection, clamp
highlights, bump mapping and invalid input on hand-built tables.

The decoder (assets/bch_model.h, skinned decode only) now returns the LUT
samplers of index slot 4 with their 256 raw entries, and each material's
LUT registers, table and sampler names and bump texture.

Assumptions (the game's runtime values are not traced)

- Lights: the field environment the viewer uses (a/0/0/3 entry 137) has
  sets of one light named light_poke_1..6 (white diffuse, specular0 and
  specular1, direction 0,-1,0). The Pokémon materials name light set 0
  (the sun), but secondary light would be zero there; the viewer lights
  them with the light_poke_1 set instead. Which set the battle uses is not
  traced.
- Specular1: the traced color loop (code.bin 0x3030a8,
  ../bch-lighting) writes specular1 as the light's own color, not its
  product with the material's (which is 0 for these materials), when its
  fourth argument is set (branch at 0x3031d8); its callers (0x2e9244,
  0x2f042c) derive that argument from bit 5 of a word reached through the
  material object at +0x288. That bit is assumed set for materials with the
  reflection LUTs enabled.
- The LUT configuration register (which LUTs a lighting config supports)
  and the per-light enables are set by the runtime: every LUT a material
  names is used.
- The light vector is the scene direction negated, rotated into view space
  by the camera; the hardware's float16 rounding is not modelled.
- Bump texels are sampled with UV0 and the unit's matrix, as the viewer
  samples every unit.

Result

In the Route 2 battle of docs/images (zone 259, seed 4, frame 238), both
battle models change: Froakie's edges catch the reflection ramps, and
Zigzagoon's fur, whose combiners read the secondary color, lightens where
the ramps rise. docs/images/native-bch-fragment-lighting.png shows the
frame before (vertex lighting only, as committed in ../vertex-lighting)
and after. Without --vertex-shader the frame is byte-identical to the
earlier viewer's (sha256 349e2217...03ef), and ../vertex-lighting's
verifier, which required Zigzagoon to stay unchanged, no longer applies to
this viewer.

Check

cc -std=c11 -O2 native/reproduction/fragment-lighting/dump_luts.c \
  native/assets/bch_model.c -lm -o native/build/dump_luts
python3 -I native/reproduction/fragment-lighting/verify_fragment_lighting.py \
  --romfs ROMFS --bridge native/build/dump_luts

Not established: the battle's own lights and light sets, the runtime's
specular1 flag, and the look against the running game.
