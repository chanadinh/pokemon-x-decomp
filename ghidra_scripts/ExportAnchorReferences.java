// Inspect supplied string addresses and export bounded evidence for referencing functions.
// @category Nintendo3DS
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.LinkedHashMap;
import java.util.HexFormat;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;

public class ExportAnchorReferences extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) throw new IllegalArgumentException("Expected output directory and hexadecimal anchor addresses");
        Path out = Path.of(args[0]);
        Files.createDirectories(out);
        JsonArray anchors = new JsonArray();
        LinkedHashMap<String, Function> functions = new LinkedHashMap<>();
        Function entry = getFunctionAt(toAddr(0x100000));
        if (entry != null) functions.put(entry.getEntryPoint().toString(), entry);
        for (int i = 1; i < args.length; i++) {
            Address address = toAddr(Long.parseUnsignedLong(args[i].replaceFirst("^0x", ""), 16));
            Function directFunction = getFunctionAt(address);
            if (directFunction != null && functions.size() < 30) {
                functions.put(directFunction.getEntryPoint().toString(), directFunction);
            }
            JsonObject anchor = new JsonObject();
            anchor.addProperty("address", address.toString());
            StringBuilder text = new StringBuilder();
            for (int j = 0; j < 256; j++) {
                int value = currentProgram.getMemory().getByte(address.add(j)) & 255;
                if (value == 0) break;
                if (value < 32 || value > 126) break;
                text.append((char)value);
            }
            anchor.addProperty("text", text.toString());
            JsonArray refs = new JsonArray();
            ReferenceIterator iter = currentProgram.getReferenceManager().getReferencesTo(address);
            int count = 0;
            while (iter.hasNext()) {
                Reference ref = iter.next(); count++;
                if (refs.size() >= 40) continue;
                JsonObject row = new JsonObject();
                Address from = ref.getFromAddress();
                row.addProperty("from", from.toString());
                row.addProperty("type", ref.getReferenceType().toString());
                Instruction instruction = getInstructionAt(from);
                if (instruction != null) row.addProperty("instruction", instruction.toString());
                Function function = getFunctionContaining(from);
                if (function != null) {
                    String key = function.getEntryPoint().toString();
                    row.addProperty("function", key);
                    row.addProperty("function_name", function.getName());
                    if (functions.size() < 30) functions.put(key, function);
                }
                refs.add(row);
            }
            anchor.addProperty("reference_count", count);
            anchor.add("references", refs);
            anchors.add(anchor);
        }
        JsonArray exported = new JsonArray();
        DecompInterface decompiler = new DecompInterface();
        try {
            if (!decompiler.openProgram(currentProgram)) throw new IllegalStateException(decompiler.getLastMessage());
            for (Function function : functions.values()) {
                monitor.checkCancelled();
                String key = function.getEntryPoint().toString();
                JsonObject row = new JsonObject();
                row.addProperty("address", key);
                row.addProperty("name", function.getName());
                row.addProperty("body_bytes", function.getBody().getNumAddresses());
                JsonArray calls = new JsonArray();
                for (Function target : function.getCalledFunctions(monitor)) {
                    JsonObject call = new JsonObject();
                    call.addProperty("address", target.getEntryPoint().toString());
                    call.addProperty("name", target.getName());
                    calls.add(call);
                }
                row.add("calls", calls);
                StringBuilder assembly = new StringBuilder();
                InstructionIterator instructions = currentProgram.getListing().getInstructions(function.getBody(), true);
                int count = 0;
                while (instructions.hasNext() && count++ < 2000) {
                    Instruction instruction = instructions.next();
                    assembly.append(instruction.getAddress()).append("  ")
                        .append(HexFormat.of().formatHex(instruction.getBytes())).append("  ")
                        .append(instruction).append('\n');
                }
                row.addProperty("assembly_truncated", instructions.hasNext());
                Files.writeString(out.resolve(key + ".asm"), assembly.toString());
                DecompileResults result = decompiler.decompileFunction(function, 30, monitor);
                boolean completed = result.decompileCompleted() && result.getDecompiledFunction() != null;
                row.addProperty("decompiled", completed);
                if (completed) {
                    Files.writeString(out.resolve(key + ".c"),
                        "/* Unreviewed Ghidra pseudocode; types and boundaries require validation. */\n" +
                        result.getDecompiledFunction().getC());
                } else row.addProperty("error", result.getErrorMessage());
                exported.add(row);
            }
        } finally { decompiler.dispose(); }
        JsonObject report = new JsonObject();
        report.add("anchors", anchors);
        report.add("functions", exported);
        report.addProperty("note", "A string reference identifies a useful lead, not proof of the function's complete purpose. Exports are bounded to 30 functions and 2000 instructions per function.");
        Files.writeString(out.resolve("references.json"), new GsonBuilder().setPrettyPrinting().create().toJson(report) + "\n");
        println("ANCHOR_EXPORT_COMPLETE functions=" + exported.size());
    }
}
