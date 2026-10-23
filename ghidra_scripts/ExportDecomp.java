// Export discovered functions and an explicitly bounded batch of unreviewed pseudocode.
// @category Nintendo3DS

import java.nio.file.Files;
import java.nio.file.Path;
import java.math.BigInteger;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.mem.MemoryBlock;

public class ExportDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 2) throw new IllegalArgumentException("Expected output directory and max-functions (0 = all)");
        Path output = Path.of(args[0]);
        int maximum = Integer.parseInt(args[1]);
        if (maximum < 0) throw new IllegalArgumentException("max-functions must be nonnegative");
        Files.createDirectories(output.resolve("pseudocode"));
        JsonArray functions = new JsonArray();
        int attempted = 0, succeeded = 0, failed = 0;
        DecompInterface decompiler = new DecompInterface();
        try {
            if (!decompiler.openProgram(currentProgram)) {
                throw new IllegalStateException("Could not open decompiler: " + decompiler.getLastMessage());
            }
            Register tmode = currentProgram.getRegister("TMode");
            FunctionIterator iter = currentProgram.getFunctionManager().getFunctions(true);
            while (iter.hasNext()) {
                monitor.checkCancelled();
                Function function = iter.next();
                JsonObject row = new JsonObject();
                String address = function.getEntryPoint().toString();
                row.addProperty("address", address);
                row.addProperty("name", function.getName());
                row.addProperty("body_bytes", function.getBody().getNumAddresses());
                row.addProperty("prototype", function.getPrototypeString(false, false));
                row.addProperty("thunk", function.isThunk());
                BigInteger mode = tmode == null ? null : currentProgram.getProgramContext().getValue(tmode, function.getEntryPoint(), false);
                row.addProperty("mode", mode == null ? "unknown" : (mode.signum() == 0 ? "ARM" : "Thumb"));
                row.addProperty("decompilation", "not_requested");
                if (!function.isExternal() && (maximum == 0 || attempted < maximum)) {
                    attempted++;
                    DecompileResults result = decompiler.decompileFunction(function, 30, monitor);
                    if (result.decompileCompleted() && result.getDecompiledFunction() != null) {
                        String relative = "pseudocode/" + address + ".c";
                        String code = "/* Unreviewed Ghidra pseudocode. Not reconstructed, buildable source. */\n" +
                            "/* " + address + " : " + function.getName() + " */\n" + result.getDecompiledFunction().getC();
                        Files.writeString(output.resolve(relative), code);
                        row.addProperty("decompilation", "success");
                        row.addProperty("file", relative);
                        succeeded++;
                    } else {
                        row.addProperty("decompilation", "failed");
                        row.addProperty("error", result.getErrorMessage());
                        failed++;
                    }
                }
                functions.add(row);
            }
        } finally {
            decompiler.dispose();
        }
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        Files.writeString(output.resolve("functions.json"), gson.toJson(functions) + "\n");
        JsonObject summary = new JsonObject();
        summary.addProperty("program", currentProgram.getName());
        summary.addProperty("language", currentProgram.getLanguageID().toString());
        summary.addProperty("code_sha256", currentProgram.getOptions("Program Information").getString("3DS input SHA256", "missing"));
        summary.addProperty("discovered_functions", functions.size());
        summary.addProperty("attempted", attempted);
        summary.addProperty("succeeded", succeeded);
        summary.addProperty("failed", failed);
        summary.addProperty("max_functions", maximum);
        JsonArray blocks = new JsonArray();
        for (MemoryBlock block : currentProgram.getMemory().getBlocks()) {
            JsonObject row = new JsonObject();
            row.addProperty("name", block.getName());
            row.addProperty("address", block.getStart().getOffset());
            row.addProperty("size", block.getSize());
            row.addProperty("read", block.isRead());
            row.addProperty("write", block.isWrite());
            row.addProperty("execute", block.isExecute());
            row.addProperty("initialized", block.isInitialized());
            blocks.add(row);
        }
        summary.add("memory_blocks", blocks);
        summary.addProperty("coverage_note", "Function discovery and types are heuristic. Counts do not establish complete program coverage. CRO modules are not included.");
        Files.writeString(output.resolve("summary.json"), gson.toJson(summary) + "\n");
        println("3DS_EXPORT_COMPLETE " + gson.toJson(summary));
    }
}
