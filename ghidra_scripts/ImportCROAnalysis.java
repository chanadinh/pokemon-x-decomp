// Import prepared analysis copies of main code and relocated CRO segments.
// Addresses assigned to CROs are synthetic analysis addresses, not runtime observations.
// @category Nintendo3DS

import java.io.ByteArrayInputStream;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Path;
import java.security.MessageDigest;
import java.util.HexFormat;
import java.util.HashMap;
import java.util.Map;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Namespace;
import ghidra.program.model.symbol.SourceType;

public class ImportCROAnalysis extends GhidraScript {
    private String clean(String name) {
        String value = name.replaceAll("[^A-Za-z0-9_.$]", "_");
        return value.isEmpty() ? "unnamed" : value;
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) throw new IllegalArgumentException("Expected prepared manifest path");
        Path manifestPath = Path.of(args[0]).toAbsolutePath();
        JsonObject manifest = JsonParser.parseString(Files.readString(manifestPath)).getAsJsonObject();
        for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
            currentProgram.getMemory().removeBlock(block, monitor);
        }
        int mapped = 0;
        for (JsonElement item : manifest.getAsJsonArray("blocks")) {
            monitor.checkCancelled();
            JsonObject segment = item.getAsJsonObject();
            long size = segment.get("size").getAsLong();
            if (size == 0) continue;
            if (size < 0 || size > 128 * 1024 * 1024) throw new IllegalArgumentException("Invalid block size");
            Address address = toAddr(segment.get("address").getAsLong());
            MemoryBlock block;
            String name = clean(segment.get("name").getAsString());
            if (!segment.has("path") || segment.get("path").isJsonNull()) {
                block = currentProgram.getMemory().createUninitializedBlock(name, address, size, false);
            } else {
                Path source = manifestPath.getParent().resolve(segment.get("path").getAsString()).normalize();
                byte[] bytes = Files.readAllBytes(source);
                String hash = HexFormat.of().formatHex(MessageDigest.getInstance("SHA-256").digest(bytes));
                if (bytes.length != size || !hash.equals(segment.get("sha256").getAsString())) {
                    throw new IllegalArgumentException("Prepared block size/hash mismatch: " + name);
                }
                block = currentProgram.getMemory().createInitializedBlock(name, address,
                    new ByteArrayInputStream(bytes), size, monitor, false);
            }
            block.setRead(segment.get("read").getAsBoolean());
            block.setWrite(segment.get("write").getAsBoolean());
            block.setExecute(segment.get("execute").getAsBoolean());
            block.setComment("Static analysis copy. CRO addresses are synthetic; see prepared manifest and relocation audit.");
            mapped++;
        }
        JsonArray seedResults = new JsonArray();
        Register tmode = currentProgram.getRegister("TMode");
        // Set every known ARM/Thumb entry context before decoding any function.
        // Export aliases can share an entry; Ghidra rejects context writes over
        // existing instructions even when the requested value is unchanged.
        Map<Address, BigInteger> modes = new HashMap<>();
        for (JsonElement item : manifest.getAsJsonArray("symbols")) {
            JsonObject row = item.getAsJsonObject();
            if (!row.get("kind").getAsString().equals("function")) continue;
            Address address = toAddr(row.get("address").getAsLong());
            BigInteger mode = row.has("mode") && !row.get("mode").isJsonNull() && row.get("mode").getAsString().equals("Thumb") ? BigInteger.ONE : BigInteger.ZERO;
            BigInteger previous = modes.putIfAbsent(address, mode);
            if (previous != null && !previous.equals(mode)) throw new IllegalArgumentException("Conflicting seed modes at " + address);
        }
        if (tmode != null) {
            for (Map.Entry<Address, BigInteger> entry : modes.entrySet()) {
                currentProgram.getProgramContext().setValue(tmode, entry.getKey(), entry.getKey(), entry.getValue());
            }
        }
        for (JsonElement item : manifest.getAsJsonArray("symbols")) {
            monitor.checkCancelled();
            JsonObject row = item.getAsJsonObject();
            Address address = toAddr(row.get("address").getAsLong());
            String module = clean(row.get("module").getAsString());
            String name = clean(row.get("name").getAsString());
            JsonObject result = row.deepCopy();
            MemoryBlock block = currentProgram.getMemory().getBlock(address);
            if (block == null) {
                result.addProperty("import_status", "unmapped_label_skipped");
                seedResults.add(result);
                continue;
            }
            Namespace namespace = currentProgram.getSymbolTable().getNamespace(module, currentProgram.getGlobalNamespace());
            if (namespace == null) namespace = currentProgram.getSymbolTable().createNameSpace(currentProgram.getGlobalNamespace(), module, SourceType.IMPORTED);
            currentProgram.getSymbolTable().createLabel(address, name, namespace, SourceType.IMPORTED);
            if (row.get("kind").getAsString().equals("function")) {
                if (!block.isExecute() || !block.isInitialized()) throw new IllegalArgumentException("Function seed outside initialized executable memory: " + address);
                boolean thumb = row.has("mode") && row.get("mode").getAsString().equals("Thumb");
                if ((address.getOffset() & (thumb ? 1 : 3)) != 0) throw new IllegalArgumentException("Unaligned function seed: " + address);
                currentProgram.getSymbolTable().addExternalEntryPoint(address);
                Function function = getFunctionAt(address);
                if (function == null) {
                    boolean decoded = disassemble(address);
                    function = decoded ? createFunction(address, null) : null;
                }
                if (function != null) {
                    // Preserve an existing function's primary name. Multiple
                    // ABI aliases may share this entry; labels retain them all.
                    result.addProperty("import_status", "function_created_or_present");
                } else {
                    result.addProperty("import_status", "function_seed_failed");
                    printerr("Could not create seed " + module + "::" + name + " at " + address);
                }
            } else result.addProperty("import_status", "label_created");
            seedResults.add(result);
        }
        Files.writeString(manifestPath.getParent().resolve("ghidra-import-seeds.json"),
            new GsonBuilder().setPrettyPrinting().create().toJson(seedResults) + "\n");
        currentProgram.getOptions("Program Information").setString("CRO analysis manifest", manifest.toString());
        currentProgram.getOptions("Program Information").setString("CRO analysis address note", "Synthetic module layout for static analysis; no game code executed");
        println("CRO_ANALYSIS_IMPORT_COMPLETE blocks=" + mapped + " symbols=" + seedResults.size());
    }
}
