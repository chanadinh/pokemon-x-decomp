Battle lighting by the models' own programs, by default, 2026-10-25

The battle models are now drawn the way ../vertex-lighting,
../fragment-lighting and ../battle-lights describe without asking for it:
their own vertex programs give the primary color and feed the PICA's
fragment lighting unit, with the materials' LUTs and the battle's
light_poke lights. --no-vertex-shader restores the earlier CPU lighting;
--vertex-shader is still accepted; giving both is an error. Only battle
models are affected (the field, the player and NPCs are drawn as before).
Tracked files contain independently authored source, synthetic tests and
metadata only.

Why now: the two assumptions the fragment lighting rested on (the light
and the specular1 rule) are traced (../battle-lights). Still approximate:
the battle camera (the viewer's own), the eye distance the view-dependent
terms see, and bump sampling with UV0.

Effect on earlier records

- The Route 2 battle frame of docs/images (zone 259, seed 4, frame 238) is
  now d7bcfa36...1772 by default and 349e2217...03ef with
  --no-vertex-shader, the two frames of ../fragment-lighting.
- Battle images in docs/images made before this change reproduce with
  --no-vertex-shader added.
- ../vertex-lighting/verify_vertex_lighting.py compared a run without the
  option against one with it; with this viewer both are lit by the
  programs, so it no longer applies.
- ../bch-catch, ../bch-save and ../bch-party verifiers pass with this
  viewer (their checks are the printed lines, unchanged).
