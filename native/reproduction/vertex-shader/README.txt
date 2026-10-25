The Pokémon models' own vertex shaders, 2026-10-25

The software PICA interpreter of ../platform now runs the vertex programs
the game's Pokémon models carry, on their real vertices and uniforms, and
the result is compared with the viewer's CPU path. Nothing executes 3DS
machine code; the programs are GPU shader bytecode stored in the models.
Tracked files contain independently authored source, synthetic tests and
metadata only.

What is read from the model (assets/bch_model.h, skinned decode only)

- The shader resources (BCH index slot 2): each a DVLB stored in the file,
  named by its dictionary entry. Froakie's model (864) has 8, named
  "0@PokePack", "1@PokePack", "2@PokePack", "0@PokeFire",
  "0@PokeFireShadow", "0@RareEffect", "0@MegaGangar", "1@MegaGangar"; the
  three PokePack names share one DVLB with three entry points.
- Each material's shader reference (material parameters +0x104), e.g.
  "1@PokePack": resource of that name, entry point 1.
- The uniforms the command streams write. The material stream (+0xc8)
  writes the texture matrices, TexcMap, MatAmbi, MatDiff, the light
  parameters (c22..c24) and ModelCenterPosition; the mesh enable stream
  PosOffs and IrScale (c6..c8); each submesh stream the bool uniforms
  (register 0x2b0: skinning mode, lighting, Phong, rim, UV maps, ...).
- The raw vertex inputs v0..v11 as the PICA vertex loader delivers them:
  stored components as numbers without the attribute scales (the program
  applies IrScale itself), absent components (0, 0, 0, 1), fixed attributes
  from float24. Registers no attribute loads are set to (0, 0, 0, 1); that
  is an assumption, and no compared output depends on it.

What the engine supplies (render/vertex_program.h)

Per draw, the game sets WrldMtx, NormMtx, ViewMtx, ProjMtx and the bone
palette UnivReg (three rows per palette entry, up to 20). Those values are
not traced in the game. Here the palette is built as render/skin.h builds it
(skeleton world matrix, times the inverse bind matrix for smooth submeshes),
NormMtx is the inverse transpose of WrldMtx, and the uniform state for a
draw is applied in stream order: the entry's own constants, then material,
mesh, submesh, then the per-draw matrices.

Result (verify_vertex_shader.py, all of a/0/0/7, rest pose)

977 models (4 are empty), 6,714 meshes, 15,974,688 vertex runs. Every
program loads and runs to END. With identity world and view matrices and a
uniform-scale projection:

- position: for all 6,704 meshes not drawn with the flame program, o0
  matches render/skin.h's posed position to 2.98e-7 (relative; float32
  against double), and o0.w is exactly 1;
- texture coordinate 0: where the program's UvMap0 bool selects mapped
  coordinates, o4.xy equals the decoded UV through the material's TexMtx0
  rows exactly; elsewhere the program generates coordinates (counted only).

Animated: the seven battle models of ../bch-battle-motion's sample (18, 21,
354, 864, 867, 869, 872) posed by their idle, physical-attack and damage
motions (slots 1, 4, 17) at the first, middle and last frames: 1,063,476
vertex runs, position error at most 3.88e-6, UV exact, o0.w exactly 1.

So the CPU skinning (a translation of SPICA's default shader) agrees with
the game's own programs on every vertex of every Pokémon in the rest pose,
and on the animated sample:
weights, palette indexing (index x 3, MOVA), the IsBoneW fourth weight and
the PosOffs / IrScale arithmetic.

What else the programs showed

- The PokePack programs light per vertex: o3 carries diffuse, Phong and rim
  terms (LightPowScale c22, DirLightVec c23), o1 is an identity normal
  quaternion and o2 the light direction in the tangent frame. The viewer's
  fragment-lighting approximation does not do this; it is the next fidelity
  step.
- They snap clip-space x in (-w, -0.99148w) to -w (c95.z), an edge fix.
  With an identity projection this showed up as 0.4% differences on
  vertices near x = -1; the comparison therefore scales the projection.
- The flame program 0@PokeFire (10 meshes in 4 models) places vertices with
  UserMatrix0..2 (c22..c24) and PressDir (c85), which the engine supplies;
  without them its output differs (listed in reference-comparison.json as
  outliers_not_comparable), so it is not compared.
- The meshes use four programs: 1@PokePack (6,254 meshes), 0@PokePack
  (446), 0@PokeFire (10) and 1@MegaGangar (4). None uses a geometry-shader
  entry (three of the eight DVLBs have one).

Tools

native/build/shader_check FILE.bch [--motion PACK --slot S] runs one model;
verify_vertex_shader.py runs all of them:

python3 -I native/reproduction/vertex-shader/verify_vertex_shader.py \
  --romfs ROMFS --check native/build/shader_check

Not established: the engine's real per-draw matrices and palette (only
that the programs agree with the CPU path given the same ones); the
programs' lighting output against the game; hardware float24 rounding (the
interpreter computes in float32).
