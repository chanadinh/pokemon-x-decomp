// Read-only field-scene evidence export. Requests are functions or references.
// The "once" mode drains requests.txt; "interactive" accepts appended requests
// until a stop file appears or 1800 polling/export iterations are reached.
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

public class ExportFieldEvidence extends GhidraScript {
    public void run() throws Exception {
        String[] initialArgs = getScriptArgs();
        String[] args = initialArgs;
        if (args.length < 2) throw new IllegalArgumentException("Output directory and function addresses required");
        Path out = Path.of(args[0]);
        Files.createDirectories(out);
        JsonArray rows = new JsonArray();
        TreeMap<String, String> literals = new TreeMap<>();
        Pattern literalPattern = Pattern.compile("\\[0x([0-9a-fA-F]+)\\]");
        DecompInterface decompiler = new DecompInterface();
        try {
            if (!decompiler.openProgram(currentProgram)) throw new IllegalStateException(decompiler.getLastMessage());
            int processed = 0;
            for (int cycle=0; cycle<1800 && !Files.exists(out.resolve("stop")); ++cycle) {
              java.util.List<String> requests = Files.readAllLines(out.resolve("requests.txt"));
              if (processed >= requests.size()) {
                if (initialArgs[1].equals("once")) break;
                Thread.sleep(1000); continue;
              }
              String request = requests.get(processed++).strip();
              if(request.isEmpty()) continue;
              String[] parts = request.split(" +");
              String target = parts[parts.length-1];
              args = new String[]{initialArgs[0],target};
              int i=1;
                monitor.checkCancelled();
                Address address = toAddr(Long.parseUnsignedLong(args[i].replaceFirst("^0x", ""), 16));
                Function function = getFunctionAt(address);
                if (function == null) function = getFunctionContaining(address);
                if (parts[0].equals("refs") || function == null) {
                  JsonArray rr = new JsonArray();
                  ReferenceIterator ri = currentProgram.getReferenceManager().getReferencesTo(address);
                  while(ri.hasNext()) { Reference r=ri.next(); JsonObject x=new JsonObject();
                    x.addProperty("from",r.getFromAddress().toString()); x.addProperty("type",r.getReferenceType().toString());
                    Function f=getFunctionContaining(r.getFromAddress()); if(f!=null)x.addProperty("function",f.getEntryPoint().toString()); rr.add(x); }
                  Files.writeString(out.resolve("refs_"+address+".json"),new GsonBuilder().setPrettyPrinting().create().toJson(rr)+"\n");
                  println("FIELD_REFS "+address+" count="+rr.size()); continue;
                }
                address=function.getEntryPoint();
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
                Files.writeString(out.resolve("functions.json"), new GsonBuilder().setPrettyPrinting().create().toJson(rows) + "\n");
                Files.writeString(out.resolve("literals.json"), new GsonBuilder().setPrettyPrinting().create().toJson(literals) + "\n");
                println("FIELD_FUNCTION_EXPORTED " + key);
            }
        } finally { decompiler.dispose(); }
        Files.writeString(out.resolve("functions.json"), new GsonBuilder().setPrettyPrinting().create().toJson(rows) + "\n");
        Files.writeString(out.resolve("literals.json"), new GsonBuilder().setPrettyPrinting().create().toJson(literals) + "\n");
        println("FIELD_EVIDENCE_EXPORT_COMPLETE count=" + rows.size());
    }
}
