// Export exact requested functions without filling the budget with callers.
// @category Nintendo3DS
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HexFormat;
import java.util.TreeMap;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
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

public class ExportFunctionSet extends GhidraScript {
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) throw new IllegalArgumentException("Output directory and function addresses required");
        Path out = Path.of(args[0]);
        Files.createDirectories(out);
        JsonArray rows = new JsonArray();
        TreeMap<String, String> literals = new TreeMap<>();
        Pattern literalPattern = Pattern.compile("\\[0x([0-9a-fA-F]+)\\]");
        DecompInterface decompiler = new DecompInterface();
        try {
            if (!decompiler.openProgram(currentProgram)) throw new IllegalStateException(decompiler.getLastMessage());
            for (int i = 1; i < args.length; ++i) {
                monitor.checkCancelled();
                Address address = toAddr(Long.parseUnsignedLong(args[i].replaceFirst("^0x", ""), 16));
                Function function = getFunctionAt(address);
                if (function == null) throw new IllegalArgumentException("No function at " + address);
                String key = address.toString();
                JsonObject row = new JsonObject();
                row.addProperty("address", key);
                row.addProperty("name", function.getName(true));
                row.addProperty("body_bytes", function.getBody().getNumAddresses());
                row.addProperty("no_return", function.hasNoReturn());
                JsonArray refs = new JsonArray();
                ReferenceIterator references = currentProgram.getReferenceManager().getReferencesTo(address);
                while (references.hasNext()) {
                    Reference ref = references.next();
                    JsonObject r = new JsonObject();
                    r.addProperty("from", ref.getFromAddress().toString());
                    r.addProperty("type", ref.getReferenceType().toString());
                    Function caller = getFunctionContaining(ref.getFromAddress());
                    if (caller != null) r.addProperty("function", caller.getEntryPoint().toString());
                    refs.add(r);
                }
                row.add("references_to", refs);
                JsonArray calls = new JsonArray();
                StringBuilder assembly = new StringBuilder();
                InstructionIterator instructions = currentProgram.getListing().getInstructions(function.getBody(), true);
                while (instructions.hasNext()) {
                    Instruction instruction = instructions.next();
                    assembly.append(instruction.getAddress()).append("  ")
                        .append(HexFormat.of().formatHex(instruction.getBytes())).append("  ")
                        .append(instruction).append('\n');
                    Matcher literal = literalPattern.matcher(instruction.toString());
                    if (literal.find() && instruction.getMnemonicString().toLowerCase().matches("ldr(eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?")) {
                        Address at = toAddr(Long.parseUnsignedLong(literal.group(1), 16));
                        byte[] bytes = new byte[4];
                        currentProgram.getMemory().getBytes(at, bytes);
                        literals.put(at.toString(), HexFormat.of().formatHex(bytes));
                    }
                    for (Reference ref : instruction.getReferencesFrom()) {
                        if (ref.getReferenceType().isCall() || ref.getReferenceType().isJump()) {
                            JsonObject call = new JsonObject();
                            call.addProperty("from", instruction.getAddress().toString());
                            call.addProperty("to", ref.getToAddress().toString());
                            call.addProperty("type", ref.getReferenceType().toString());
                            calls.add(call);
                        }
                    }
                }
                row.add("flow_references", calls);
                Files.writeString(out.resolve(key + ".asm"), assembly.toString());
                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                boolean complete = result.decompileCompleted() && result.getDecompiledFunction() != null;
                row.addProperty("decompiled", complete);
                if (complete) Files.writeString(out.resolve(key + ".c"),
                    "/* Unreviewed Ghidra pseudocode; types require assembly review. */\n" + result.getDecompiledFunction().getC());
                else row.addProperty("error", result.getErrorMessage());
                rows.add(row);
            }
        } finally { decompiler.dispose(); }
        Files.writeString(out.resolve("functions.json"), new GsonBuilder().setPrettyPrinting().create().toJson(rows) + "\n");
        Files.writeString(out.resolve("literals.json"), new GsonBuilder().setPrettyPrinting().create().toJson(literals) + "\n");
        println("EXACT_FUNCTION_EXPORT_COMPLETE count=" + rows.size());
    }
}
