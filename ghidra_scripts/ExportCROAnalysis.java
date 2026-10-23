// Export candidate functions across imported CROs, with bounded focused pseudocode.
// @category Nintendo3DS

import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.mem.MemoryBlock;

public class ExportCROAnalysis extends GhidraScript {
    private String moduleAt(long address, JsonArray blocks) {
        for (JsonElement item : blocks) {
            JsonObject block = item.getAsJsonObject();
            long start = block.get("address").getAsLong();
            long size = block.get("size").getAsLong();
            if (address >= start && address - start < size) return block.get("module").getAsString();
        }
        return "unmapped";
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 3) throw new IllegalArgumentException("Expected prepared manifest, output dir, per-target export limit");
        JsonObject manifest = JsonParser.parseString(Files.readString(Path.of(args[0]))).getAsJsonObject();
        JsonArray blocks = manifest.getAsJsonArray("blocks");
        Path output = Path.of(args[1]);
        int limit = Integer.parseInt(args[2]);
        if (limit < 1 || limit > 500) throw new IllegalArgumentException("Export limit must be 1..500");
        Files.createDirectories(output);
        JsonArray functions = new JsonArray();
        Map<String, JsonObject> stats = new HashMap<>();
        Map<String, Integer> attempts = new HashMap<>();
        Register tmode = currentProgram.getRegister("TMode");
        DecompInterface decompiler = new DecompInterface();
        try {
            if (!decompiler.openProgram(currentProgram)) throw new IllegalStateException(decompiler.getLastMessage());
            FunctionIterator iterator = currentProgram.getFunctionManager().getFunctions(true);
            while (iterator.hasNext()) {
                monitor.checkCancelled();
                Function function = iterator.next();
                String module = moduleAt(function.getEntryPoint().getOffset(), blocks);
                JsonObject summary = stats.computeIfAbsent(module, k -> {
                    JsonObject s = new JsonObject();
                    s.addProperty("candidate_functions", 0); s.addProperty("attempted", 0);
                    s.addProperty("succeeded", 0); s.addProperty("failed", 0); return s;
                });
                summary.addProperty("candidate_functions", summary.get("candidate_functions").getAsInt() + 1);
                JsonObject row = new JsonObject();
                String address = function.getEntryPoint().toString();
                row.addProperty("module", module); row.addProperty("address", address);
                row.addProperty("name", function.getName(true));
                row.addProperty("body_bytes", function.getBody().getNumAddresses());
                row.addProperty("prototype", function.getPrototypeString(false, false));
                row.addProperty("thunk", function.isThunk());
                BigInteger mode = tmode == null ? null : currentProgram.getProgramContext().getValue(tmode, function.getEntryPoint(), false);
                row.addProperty("mode", mode == null ? "unknown" : mode.signum() == 0 ? "ARM" : "Thumb");
                row.addProperty("decompilation", "not_requested");
                boolean focused = module.equals("DllBattle") || module.equals("DllField") || module.equals("DllBattle.cro") || module.equals("DllField.cro");
                // CRO text begins with many tiny import veneers. Skip these so
                // an address-ordered batch reaches module implementation code.
                boolean veneer = function.getBody().getNumAddresses() <= 8 &&
                    getInt(function.getEntryPoint()) == 0xe51ff004;
                if (function.isThunk() || veneer) row.addProperty("export_skip_reason", "import veneer or recognized thunk");
                if (focused && attempts.getOrDefault(module, 0) < limit && !function.isExternal() && !function.isThunk() && !veneer) {
                    attempts.put(module, attempts.getOrDefault(module, 0) + 1);
                    summary.addProperty("attempted", summary.get("attempted").getAsInt() + 1);
                    DecompileResults result = decompiler.decompileFunction(function, 30, monitor);
                    if (result.decompileCompleted() && result.getDecompiledFunction() != null) {
                        String safeModule = module.replaceAll("[^A-Za-z0-9_.-]", "_");
                        Path directory = output.resolve("pseudocode").resolve(safeModule);
                        Files.createDirectories(directory);
                        Path destination = directory.resolve(address + ".c");
                        String code = "/* Unreviewed Ghidra pseudocode from a relocated STATIC ANALYSIS COPY. */\n" +
                            "/* Module " + module + "; synthetic address " + address + "; not buildable source. */\n" + result.getDecompiledFunction().getC();
                        Files.writeString(destination, code);
                        row.addProperty("decompilation", "success");
                        row.addProperty("file", output.relativize(destination).toString());
                        summary.addProperty("succeeded", summary.get("succeeded").getAsInt() + 1);
                    } else {
                        row.addProperty("decompilation", "failed"); row.addProperty("error", result.getErrorMessage());
                        summary.addProperty("failed", summary.get("failed").getAsInt() + 1);
                    }
                }
                functions.add(row);
            }
        } finally { decompiler.dispose(); }
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        Files.writeString(output.resolve("functions.json"), gson.toJson(functions) + "\n");
        JsonObject summary = new JsonObject();
        summary.addProperty("candidate_functions", functions.size());
        summary.addProperty("per_target_export_limit", limit);
        summary.addProperty("export_selection", "First functions by address in Battle/Field, excluding recognized thunks and tiny LDR-pc import veneers");
        summary.addProperty("quality", "Unreviewed automatic analysis; synthetic CRO addresses; not buildable source or complete coverage");
        summary.add("by_module", gson.toJsonTree(stats));
        JsonArray memory = new JsonArray();
        for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
            JsonObject row = new JsonObject();
            row.addProperty("name", block.getName()); row.addProperty("address", block.getStart().getOffset());
            row.addProperty("size", block.getSize()); row.addProperty("read", block.isRead());
            row.addProperty("write", block.isWrite()); row.addProperty("execute", block.isExecute());
            row.addProperty("initialized", block.isInitialized()); memory.add(row);
        }
        summary.add("memory_blocks", memory);
        Files.writeString(output.resolve("summary.json"), gson.toJson(summary) + "\n");
        println("CRO_ANALYSIS_EXPORT_COMPLETE functions=" + functions.size());
    }
}
