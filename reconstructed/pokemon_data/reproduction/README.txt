Pokemon data evidence reproduction

The retained evidence was exported with Ghidra 12.1.4 and JDK 21 from the
existing combined Pokemon X analysis project. Game inputs, installed tools,
and saved projects are local artifacts excluded from Git and the source ZIP.

From the complete workspace root on Linux:

python3 reconstructed/pokemon_data/reproduction/export_data.py \
  --project projects/cro_20261023T150920079989Z.gpr \
  --output outputs/pokemon-data/reproduce-new

The output directory must not exist. Supply --workspace, --ghidra, and --jdk
when tools are installed elsewhere. reviewed-command.json records the actual
headless invocation used to check the evidence.

The launcher opens main_text.bin with -noanalysis and -readOnly, then exports
42 requested functions. No repair is needed for this function set and no
project changes are saved. The reproduced 84 assembly/pseudocode files were
compared byte for byte with the retained evidence.

evidence/provenance.json binds 3,360 instruction bytes, 76 literal bytes,
and 128 block-order table bytes to the uploaded executable's SHA-256.
evidence/block_order.json records the observed table as numeric rows. The
host implementation reads the table through logical memory adapters; its
tests supply deliberately synthetic tables and do not load game assets.

No original game code executes during this workflow. Ghidra C retains its
inferred types and is not the host implementation. Export 00167db0 documents
the remaining growth-resource-selection callback, not reconstructed code.
