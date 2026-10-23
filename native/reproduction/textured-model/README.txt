Textured static-model preview milestone
Recorded 2026-10-23; final build/source hashes and integration results are
recorded separately in this directory by the final validation pass.

Scope

The native model viewer adds an orthographic, unlit texture-times-vertex-color
preview for the explicitly supported CGFX material subset. It uses decoded
RGBA images, UV0, normalized vertex colors, material alpha/depth/cull/sampler
state, and replace or additive source-alpha blending. This is a software
inspection view, not a PICA shader, lighting, camera, or game-frame emulator.
Animation, skeletal deformation, arbitrary TEV programs, extra texture units,
nonidentity UV transforms, stencil, polygon offset, unsupported material
commands and mip-chain rendering remain outside the accepted subset. The
parser rejects unsupported state instead of inventing its behavior.

The preview orders draws stably by material translucency kind, then render
priority, then original resource object order. Each draw preserves its triangle
index order. This policy is based on the pinned reference viewer's layer and
priority behavior; it is not a reconstruction of Pokemon X's game draw order
or camera-dependent transparency sorting. See docs/native-materials.md for
the parser contract, offsets, supported states, and the distinction between
format evidence and preview policy.

The real local sample is archive a/1/0/2, entry0, subentry0, model0: the static
model labeled collision02, with 767 vertices, 936 triangles and six shapes.
That stored label does not establish a map or gameplay role. Asset
bytes, downloaded source and rendered screenshots remain in ignored local
outputs; no game content is included in this milestone's tracked files.

Raster contract

The portable C11 API is native/render/raster.h, with no SDL dependency.
Targets own tightly packed top-down RGBA8 and double-precision depth arrays.
Call clear before the first draw. Free accepts a NULL or zero target. A fresh
target is zeroed on allocation failure; decoded source images are borrowed
with an explicit byte span, checked against width*height*4 before sampling.

Screen y increases downward, and smaller z is closer. UV(0,0) addresses the
decoded image's top left. Root application projection negates camera-space z
and flips CGFX V to 1-V for these top-down images. The parser keeps original
scaled UV0 values; the orientation conversion happens once in the caller.

Coverage snaps each screen x/y coordinate to a 1/256-pixel grid with nearest
rounding, ties away from zero. Pixel centers are x+0.5,y+0.5. Exact int64 edge
functions implement the top-left fill rule: after orientation normalization
to positive area, an edge is inclusive when dy<0, or dy==0 and dx>0. Positive
signed area is clockwise in y-down space. Shared edges consequently belong
to exactly one triangle. Vertex attributes are affinely interpolated over
the snapped triangle; there is no perspective correction or multisampling.
The app declares counterclockwise screen winding as front-facing for its
camera projection, and maps CGFX front/back culling explicitly.

All vertex components must be finite. Absolute x/y values may not exceed
1,000,000; absolute z/u/v values may not exceed 1,000,000,000,000. Vertex RGBA
channels are clamped to [0,1] before interpolation. Finite offscreen geometry
is handled by clipping the integer pixel scan bounds to the viewport;
degenerate, culled and wholly offscreen triangles succeed without drawing.

Viewport dimensions are limited to 4096 on each axis and 4,194,304 pixels in
total. A clear resets a 67,108,864 candidate-pixel budget. Every visible
nondegenerate triangle consumes its clipped integer bounding-box area,
including candidates subsequently rejected by coverage/depth/alpha tests.
Exceeding the budget rejects the complete triangle before writes or budget
consumption. Other argument/limit errors also leave this triangle's output
and budget unchanged. The app additionally limits unique decoded textures
to 64 MiB before decoding each image.

Nearest sampling uses floor(u*width), floor(v*height) after address reduction.
Bilinear sampling uses texel-center coordinates u*width-0.5,v*height-0.5 and
four separately addressed neighbors. Clamp-to-edge, repeat and mirrored
repeat are supported independently on U/V. Filtering uses straight RGBA
channels, then texture RGBA multiplies interpolated vertex RGBA. Floating
precision is retained through sampling/modulation/blending, with final
clamped RGBA8 conversion rounded to nearest. One base mip is supported.

All eight standard compare functions are implemented for depth and alpha.
Depth test and depth write are independent. Alpha compares the rounded source
alpha byte with the material's byte reference before any color/depth write;
discarded fragments cannot update depth. Optional RGBA write-mask bits apply
after blending. The default state uses LESS depth test/write, ALWAYS alpha,
no culling, replace blending and all four color writes.

Blend modes are separate contracts:
  REPLACE: destination becomes source.
  SOURCE_ALPHA: every RGBA channel becomes src*srcA + dst*(1-srcA).
  SOURCE_OVER: straight-RGBA Porter-Duff source-over, including normalized RGB
    when destination alpha is not opaque; zero output alpha yields zero RGB.

Four real sample materials use PICA blend word0x76760000: additive equations
and source-alpha/one-minus-source-alpha factors for BOTH RGB and alpha. They
therefore use SOURCE_ALPHA, whose output alpha is srcA*srcA+dstA*(1-srcA).
The two replace materials use word0x01010000. Treating the first state as
SOURCE_OVER would give different alpha and is not done. PICA compare values
are mapped explicitly; in particular value4 is LESS and value5 is LEQUAL.

Primary references

Standards/vendor pages consulted 2026-10-23 (not repository commit pins):
  Microsoft, pixel-center sampling and top-left triangle fill convention:
  https://learn.microsoft.com/en-us/windows/uwp/graphics-concepts/introduction-to-rasterization-rules
  https://learn.microsoft.com/en-gb/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules
  Khronos, OpenGL1.4 specification, texture addressing/filtering and blending:
  https://registry.khronos.org/OpenGL/specs/gl/glspec14.pdf
  NVIDIA, texel-center/pixel-center mapping explanation:
  https://developer.download.nvidia.com/assets/gamedev/docs/D3D_Texel_to_Pixel_Mapping.pdf

The chosen 1/256 grid, caps, rounding and affine preview behavior are explicit
local contracts; these references do not establish PICA coverage equivalence.

Material/texture reference implementation: gdkchan/SPICA, pinned commit
bd29a7828595d7839cda2ac61c76bb63f9071250:
  https://github.com/gdkchan/SPICA/tree/bd29a7828595d7839cda2ac61c76bb63f9071250
  SPICA/Formats/CtrGfx/Model/Material/GfxMaterial.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxRasterization.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxTextureCoord.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxTextureMapper.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxTextureReference.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxTextureSamplerStd.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxFragOpDepth.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxFragOpBlend.cs
  SPICA/Formats/CtrGfx/Model/Material/GfxAlphaTest.cs
  SPICA/PICA/Commands/PICAAlphaTest.cs
  SPICA/PICA/Commands/PICADepthColorMask.cs
  SPICA/PICA/Commands/PICABlendFunction.cs
  SPICA/PICA/Commands/PICATestFunc.cs
  SPICA/PICA/Commands/PICATextureWrap.cs

Row-orientation evidence at that same commit:
  SPICA/PICA/Converters/TextureConverter.cs, raw decoded row reversal
  SPICA/PICA/Converters/TextureCompression.cs, ETC decoded row reversal
  SPICA/Formats/CtrH3D/Texture/H3DTexture.cs, converter use
  SPICA.Rendering/Texture.cs, upload to OpenGL
The local decoders preserve their existing top-down row contract; conversion
in model_app.c uses V->1-V to match that reference's texture-coordinate sense.

Preview ordering evidence at the same commit:
  SPICA/Formats/CtrGfx/Gfx.cs, translucency kind to reference layer
  SPICA.Rendering/Model.cs, stable priority ordering
  SPICA.Rendering/Model.cs, layers 0..3 iteration
The reference's layer1 reversal workaround is not evidence of game ordering
and is not applied as a universal rule here.

Validation handoff

native/render/test_raster.c uses fixed expected pixels, not implementation
round trips. It covers asymmetric UV orientation, exact affine color/depth
values, negative and positive wrap coordinates, nearest/bilinear filtering,
repeat/mirror seams, alpha-byte rounding and discard, both alpha blend
formulas, partially transparent destinations, color masks, all eight depth
and alpha compare functions, independent depth test/write, both winding
signs, every cull mode, shared-edge coverage in both draw orders, top/left
inclusion, half-subpixel rounding, viewport clipping, degenerate/offscreen
geometry, nonfinite values, malformed image spans/state, and atomic budget
rejection/reset.

The isolated raster suite passed strict C11 at -O2 with
-Wall -Wextra -Werror -pedantic. It also passed an -O1 AddressSanitizer plus
UndefinedBehaviorSanitizer build with ASAN_OPTIONS=detect_leaks=0, the existing
local environment setting. Test binaries were written under /tmp to avoid
concurrent writes to the integrated viewer build.

Final integration inventory/results are recorded by the root validation pass:
  - Nine native C core executables, including scene parser and raster tests.
  - Model viewer runtime suite: ten cases.
  - Existing asset viewer and Python container suites remain in the test set.
  - Eight previous reconstruction checkpoints: 539 manifest entries.
  - Previous native baseline: 50 files pinned by baseline.json at commit
    cdff6f160a8934c97f0859edccc812404e86fd01.
  - Final integrated counts, sanitizer runs, sample checks, source hashes and
    preserved-baseline verification: see the final validation record added
    beside this file; do not infer full integration success solely from the
    isolated raster result above.

No old reproduction records or reconstructed modules were edited by the
raster implementation. A read-only model_app.c review checked texture and
frame ownership, failure cleanup, 64 MiB image budgeting, enum mappings,
projection/UV conventions, resizing and exclusive-create BMP capture.

Final integrated validation

All 14 recorded build/check commands passed. Nine C core suites, eight Python
container tests, seven texture-viewer tests and ten model-viewer tests passed.
All eight reconstruction suites and 38 analysis tests passed (two optional
dependency skips). Geometry/scene/raster ASan+UBSan suites and the complete
model runtime suite passed; leak detection was disabled as described above.
The actual textured desktop preview and Windows-to-WSL launcher both passed.
All 539 reconstruction hashes and 19 prior native reproduction records remain
unchanged. validation.json records commands, outputs and tested source hashes;
rendered-sample.json records the inspected viewport and local capture hashes.
