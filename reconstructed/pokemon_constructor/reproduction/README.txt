Constructor evidence reproduction

The evidence is from Ghidra 12.1.4 with JDK 21 in the existing combined
Pokemon X analysis project. The game inputs, installed tools, and project
are local artifacts excluded from the source repository and source ZIP.

From the complete workspace root on Linux:

python3 reconstructed/pokemon_constructor/reproduction/export_constructor.py \
  --project projects/cro_20261023T150920079989Z.gpr \
  --output outputs/pokemon-constructor/reproduce-new

The output directory must not exist. Use --workspace, --ghidra, and --jdk
when installations are in different locations. The saved command JSON
records the exact invocation for the reviewed export.

The launcher processes the existing main_text.bin program with -noanalysis
and -readOnly. The guarded script checks original instruction/literal words,
marks zero-fill helper 00301a9c as returning, removes two erroneous terminal
call overrides, and restores 0039a8a4's body through 0039a98f. It exports
19 functions, then discards the temporary project changes.

No original game code executes. C exports retain Ghidra's inferred types;
the host reconstruction is based on the assembly and independent review.
evidence/provenance.json binds each instruction and selected literal to the
uploaded code.bin SHA-256. The 528-byte 0014f6b8 export and larger state
helpers document remaining callback boundaries, not completed host code.
