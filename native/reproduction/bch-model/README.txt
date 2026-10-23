BCH static model and external texture inspection, 2026-10-23

Scope: independently authored Python model/command readers, synthetic tests,
and selected-input metadata comparisons. No original asset payload, geometry
arrays, external implementation or rendered-game equivalence is included.
The previous BCH index work was already present in the working tree; it now
also provides reusable GR extraction and expanded bounds/CLI tests.

Checked region 165: 12 meshes, 2356 stored vertices, 4992 raw indices. An
independent owned-copy relocation decoder agrees with the production pointer
resolver on all positions, UV0, indices, strides and bounds. Area 75 supplies
19 world textures, matching all 16 nonnull references across 12 materials.

Files

field165-model.json: production metadata, exact texture-name matches and
per-mesh numeric hashes. No vertex/UV/index arrays.
reference-comparison.json: independent numeric comparison and checker hash.
verify_geometry_reference.py: standalone selected-baseline checker, requiring
the exact BCH hash/size. It imports no production parser and writes metadata.
area75-texture-association.json: area/archive/section hashes, texture metadata,
material references and pinned reference provenance.
verify_area_textures.py: selected-baseline replay from local RomFS using the
existing GARC index and native LZ decoder; output is exclusive metadata, with
optional extraction into a new explicitly requested local directory.
area-texture-evidence.txt: exact selection, layouts, source references and limits.
validation.json / validation.txt: commands, suite counts, results and log evidence.

Reproduce from local inputs (run at repository root)

make -C native test
cc -shared -fPIC -O2 native/assets/lz.c -o /tmp/pokemon-x-bch-lz.so
python3 native/reproduction/bch-model/verify_area_textures.py \
  --romfs /path/to/romfs --lz-library /tmp/pokemon-x-bch-lz.so \
  --output new-area-report.json --extract-dir new-local-bch-inputs
python3 native/tools/inspect_bch_model.py \
  --file new-local-bch-inputs/field165.bch \
  --texture-bch new-local-bch-inputs/area75-world-textures.bch
python3 native/reproduction/bch-model/verify_geometry_reference.py \
  --file new-local-bch-inputs/field165.bch --output new-reference.json

Compare each mesh's positions_sha256, uv0_sha256 and indices_sha256 with the
production report. All twelve must match. Per-mesh counts, stride and bounds
must also agree. The tracked reference-comparison.json records this check.
The selected-baseline scripts intentionally assert exact data/provenance; they
are diagnostic reproduction scripts, not hardened parsers for arbitrary files.
The general CLI readers have separate malformed-input and budget tests.

Optional local inspection export

python3 native/tools/inspect_bch_model.py \
  --file new-local-bch-inputs/field165.bch \
  --triangle-list-obj new-field.obj

Existing OBJ files and symlinks are refused. No MTL, textures, game visibility,
lighting or shader behavior is exported. Original asset-derived outputs belong
outside Git. Use outputs/ for local artifacts; never add the extracted BCHs or
OBJ to the repository.

Limits and continuation

The selected primitive writes use mask 8/parameter 0 after draw, leaving bits
8-9 unknown. The optional OBJ deliberately groups index triples (1664 faces)
as an inspection interpretation. Original draw topology still needs validation.
Four materials use projection_dummy plus their ordinary texture in unit 1;
projected coordinates and effects are not replaced by a guessed ordinary map.
Name matches are not runtime binding proof. Full BCH validation, native BCH
rendering, shaders/materials, original area consumers and player entry remain
pending. No new game-logic milestone or playable game is claimed.

Reviewed format references, with independently authored implementations:
https://github.com/gdkchan/SPICA/tree/bd29a7828595d7839cda2ac61c76bb63f9071250
https://github.com/Rynbo/CTRMap/tree/4b780a7bc32a75a7f31c162829b086355caf32f2
See docs/native-bch.md for the complete accepted subset and budgets.
