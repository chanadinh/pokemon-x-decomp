Battle models lit by their own vertex programs, 2026-10-25

--vertex-shader (with --zone) gives the battle models the primary color the
PICA would hand their combiners: each mesh's own vertex program runs in the
software interpreter for every drawn index (render/vertex_program.h; see
../vertex-shader), and its color output o3 replaces the stored vertex color.
Positions and UVs stay those of the CPU path, which agree with the programs
(../vertex-shader). The option is off by default. Tracked files contain
independently authored source, synthetic tests and metadata only.

What it changes

The PokePack programs light each vertex: with LightingEnable they put
separate diffuse, Phong and rim terms in o3's channels (LightPowScale c22,
DirLightVec c23 and the material colors, from the material's own command
stream). Materials whose combiners read the primary color (TEV source 0)
then show those terms; without the option they read the stored vertex
color, usually white, which is why Froakie's head and body were blown out.
Materials that never read source 0 look the same either way.

In the Route 2 battle of docs/images (zone 259, seed 4, frame 238):
Froakie's six materials that read source 0 change (15,828 pixels, all in
the partner's half); Zigzagoon's (model 354) read no source 0 and its
programs run with lighting off, so it is unchanged. Both runs print the same
lines. docs/images/native-bch-vertex-lighting.png shows Froakie without
(left) and with (right) the option.

Assumptions (the engine's per-draw values are not traced)

- WrldMtx is the battle placement (yaw, uniform scale, translation);
  ViewMtx is the viewer's camera rotation with the eye 1000 units in front,
  so the view-dependent terms (Phong, rim) see a nearby perspective eye,
  while the viewer draws orthographically. ProjMtx is the identity (o0 is
  not used). The bone palette is the current motion pose, built as in
  render/skin.h.
- DirLightVec and the light parameters are the material stream's values;
  the game may override them per scene.
- The fragment stage is unchanged: its lighting still uses the scene's
  directional lights and interpolated normals, not the programs' identity
  normal quaternion and tangent-space light vector (o1, o2).

Cost: for this battle about 39,000 extra vertex-program runs a frame; the
240-frame capture took the same wall time with and without the option
here (about 16 s, dominated by software rasterization).

Check

python3 -I native/reproduction/vertex-lighting/verify_vertex_lighting.py \
  --romfs ROMFS --code-bin CODE.BIN --viewer native/build/pokemon_model_viewer

writes reference-comparison.json (capture hashes, changed pixel box, the
materials reading source 0).

Not established: that the game's battle lighting looks like this (no
running-game comparison); the engine's light and camera values.
