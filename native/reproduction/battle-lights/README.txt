The battle's Pokémon lights and the specular1 rule, 2026-10-25

The fragment lighting record (../fragment-lighting) rested on two
assumptions: which light the battle gives the Pokémon, and when the game
writes a light's specular1 color unmultiplied. Both are now traced in the
game's own code. verify_battle_lights.py checks every instruction and data
word cited here (28 in code.bin, 10 in DllBattle.cro, the relocation and the
strings). Tracked files contain independently authored source and metadata
only; the binaries are not included.

The lights (DllBattle.cro, the battle module in the RomFS)

At 0x38f54 the battle loads a literal that the module's internal relocation
table resolves to rodata 0xd8c8c. Base + 0xc8 is the string "light_poke_1".
The loop at 0x38f60..0x38fb4 copies that 16-byte name, adds the loop index
to its byte 11 (so '1'..'6'), looks the name up in the scene and stores the
result per index (+0xb0 + 4k, six of them); then base + 0xd8,
"light_eff_obj", is looked up the same way. The only scene in the RomFS,
scene_environment_field (a/0/0/3 entry 137, section 0), has exactly these
lights, each alone in its own light set (sets 5..10, 11), with white
diffuse, specular0 and specular1, no ambient, direction (0, -1, 0). So the
battle lights its Pokémon with light_poke_N from the field scene, not with
the sun of light set 0 that the materials name.

Not traced: which battle position takes which N (the viewer gives the
partner light_poke_1 and the wild Pokémon light_poke_2; all six are
identical in the file), and whether the battle moves the lights at run
time (the viewer keeps the file's direction).

The specular1 rule (code.bin, the graphics library)

The function that writes a material's lighting state (0x2e8a30, call at
0x2e9244) reads a flags word through the material object's relative pointer
at +0x288 (+0x10). In the lighting config it builds (0x2e9030..0x2e9074)
bit 5 of that word decides the reflection LUTs: set, config1 bits 20..22 are
0 (R, G, B enabled); clear, they are 7 (disabled). The same bit is computed
again at 0x2e8f54..0x2e8f6c and passed as the fourth argument of the color
loop 0x302edc, whose branch at 0x3031d8 then writes specular1 as the light's
color alone rather than the product with the material's specular1 that the
loop otherwise writes (0x3031fc..0x303208, ../bch-lighting). So specular1
is the light's own color exactly when the reflection LUTs are enabled, as
the DMP lighting model the library implements (its uniform table names
dmp_LightEnv.lutEnabledRefl, GL_BOOL uniform 0xea) prescribes. The viewer's
rule (render/frag_light.h use in model_app.c) is that: reflection on (the
material's fragment flag bit 4), specular1 = the light's.

Still not traced: how the library fills the flags word from the material
(the viewer reads the material's own fragment flags), and the lighting
config's LUT configuration value (the viewer uses every LUT a material
names).

Viewer change

The partner and wild Pokémon now take light_poke_1 and light_poke_2 by
name, and the code comment states the traced rule. Output is unchanged
(the lights are identical): the Route 2 battle frame with --vertex-shader
is still d7bcfa36...1772.

Check

python3 -I native/reproduction/battle-lights/verify_battle_lights.py \
  --code-bin CODE.BIN --cro ROMFS/DllBattle.cro
