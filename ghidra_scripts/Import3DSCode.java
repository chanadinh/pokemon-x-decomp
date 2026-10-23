// Map a validated, decompressed 3DS ExeFS code file using its matching ExHeader.
// Run as a preScript on a fresh raw-binary ARM import, before auto-analysis.
// @category Nintendo3DS

import java.io.ByteArrayInputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.security.MessageDigest;
import java.util.HexFormat;
import java.math.BigInteger;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;

public class Import3DSCode extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) throw new IllegalArgumentException("Expected manifest.json path");
        JsonObject manifest = JsonParser.parseString(Files.readString(Path.of(args[0]))).getAsJsonObject();
        JsonObject codeInfo = manifest.getAsJsonObject("code");
        byte[] code = Files.readAllBytes(Path.of(codeInfo.get("path").getAsString()));
        String hash = HexFormat.of().formatHex(MessageDigest.getInstance("SHA-256").digest(code));
        if (!hash.equals(codeInfo.get("sha256").getAsString())) {
            throw new IllegalArgumentException("Input changed after validation");
        }
        // This script is only for a fresh import; discard the generic raw mapping.
        for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
            currentProgram.getMemory().removeBlock(block, monitor);
        }
        for (JsonElement item : manifest.getAsJsonArray("segments")) {
            JsonObject segment = item.getAsJsonObject();
            long size = segment.get("size").getAsLong();
            if (size == 0) continue;
            String name = segment.get("name").getAsString();
            Address start = toAddr(segment.get("address").getAsLong());
            MemoryBlock block;
            if (segment.get("offset").isJsonNull()) {
                block = currentProgram.getMemory().createUninitializedBlock(name, start, size, false);
            } else {
                int offset = segment.get("offset").getAsInt();
                block = currentProgram.getMemory().createInitializedBlock(name, start,
                    new ByteArrayInputStream(code, offset, Math.toIntExact(size)), size, monitor, false);
            }
            block.setRead(segment.get("read").getAsBoolean());
            block.setWrite(segment.get("write").getAsBoolean());
            block.setExecute(segment.get("execute").getAsBoolean());
            block.setComment("Mapped from validated 3DS ExHeader; code SHA-256 " + hash);
            println("Mapped " + name + " at " + start + " size " + size);
        }
        Address entry = toAddr(manifest.get("entry_point").getAsLong());
        Register tmode = currentProgram.getRegister("TMode");
        if (tmode != null) currentProgram.getProgramContext().setValue(tmode, entry, entry, BigInteger.ZERO);
        createLabel(entry, "ctr_entry", true, SourceType.IMPORTED);
        currentProgram.getSymbolTable().addExternalEntryPoint(entry);
        if (!disassemble(entry)) throw new IllegalStateException("Could not disassemble the ARM entry point");
        if (createFunction(entry, "ctr_entry") == null && getFunctionAt(entry) == null) {
            throw new IllegalStateException("Could not create the entry function");
        }
        currentProgram.getOptions("Program Information").setString("3DS input SHA256", hash);
        currentProgram.getOptions("Program Information").setString("3DS import manifest", manifest.toString());
        println("3DS_IMPORT_COMPLETE " + hash);
    }
}
