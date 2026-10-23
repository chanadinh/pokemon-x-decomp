BCH resource index continuation, 2026-10-23

Scope: independently authored read-only Python inspector for BCH compatibility
7/7 resource indexes. No original payload bytes or external source code included.
Input: checked a/0/4/1 entry165, decompressed GR, section1. Report offsets are
relative to the BCH section. field165-index.json records SHA-256 and metadata.
The GR section is127232 bytes including trailing alignment; the BCH relocation
section ends at127108. Trailing bytes are not interpreted or validated.

Reference: SPICA commit bd29a7828595d7839cda2ac61c76bb63f9071250
https://github.com/gdkchan/SPICA/tree/bd29a7828595d7839cda2ac61c76bb63f9071250
Reviewed SPICA/Formats/CtrH3D/{H3DHeader,H3D,H3DDict,H3DPatriciaTree,
H3DPatriciaTreeNode,H3DRelocator}.cs for version7 header, category order,
section-relative offsets and dictionary nodes. No code copied.

Reproduce:
python3 native/tools/inspect_bch.py --file /path/to/selected.gr --gr-section 1
python3 -m unittest discover -s native/tests -p test_bch.py -v
make -C native test

The initial five synthetic tests exercise identity, read-only behavior, truncated headers,
section overlap/bounds, resource budgets, dictionary/object bounds, child indices,
name termination and unsupported versions. The inspector deliberately does not
walk the Patricia lookup algorithm or validate relocation entries/object bodies.
No BCH rendering, texture binding or running-game equivalence is claimed.
The zero embedded texture count makes external area texture selection the next
investigation, not a proven association. Prior reproduction records untouched.

Current coverage: twelve synthetic index/GR tests, including reusable GR section
extraction, CLI error reporting, and exact 1024-byte name boundaries. The initial
five-test validation record above is preserved as historical evidence.
