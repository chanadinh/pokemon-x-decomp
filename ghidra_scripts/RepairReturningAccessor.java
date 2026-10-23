// Repair one proven false no-return classification and directly affected calls.
// Exact input instructions are checked before changing analysis metadata.
// @category Nintendo3DS

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.FlowOverride;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;

public class RepairReturningAccessor extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) throw new IllegalArgumentException("Expected output JSON path");
        Address target = toAddr(0x003dcfc8L);
        int[] words = {0xe92d4010,0xe59f4020,0xe5940000,0xe3500000,0x1a000003,
            0xe3a02000,0xe1a01002,0xe1a00002,0xe1a00000,0xe5940000,0xe8bd8010};
        for (int i=0;i<words.length;i++) {
            if (getInt(target.add(i*4)) != words[i]) throw new IllegalStateException("Returning helper bytes differ");
        }
        currentProgram.getOptions("Analyzers").setBoolean("Non-Returning Functions - Discovered", false);
        Set<Address> targets = new HashSet<>();
        JsonArray corrected = new JsonArray();
        FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
        while (functions.hasNext()) {
            Function f = functions.next();
            Function destination = f.isThunk() ? f.getThunkedFunction(true) : f;
            if (destination != null && destination.getEntryPoint().equals(target)) {
                JsonObject row = new JsonObject();
                row.addProperty("address", f.getEntryPoint().toString());
                row.addProperty("name", f.getName(true));
                row.addProperty("was_no_return", f.hasNoReturn());
                row.addProperty("thunk", f.isThunk());
                corrected.add(row); targets.add(f.getEntryPoint()); f.setNoReturn(false);
            }
        }
        if (!targets.contains(target)) throw new IllegalStateException("Helper function not found");
        Set<Address> callsites = new LinkedHashSet<>();
        Set<Function> callers = new LinkedHashSet<>();
        for (Address address : targets) {
            for (Reference ref : getReferencesTo(address)) {
                if (!ref.getReferenceType().isCall()) continue;
                Instruction instruction = getInstructionAt(ref.getFromAddress());
                if (instruction == null) continue;
                callsites.add(instruction.getAddress());
                Function caller = getFunctionContaining(instruction.getAddress());
                if (caller != null) callers.add(caller);
            }
        }
        JsonArray restored = new JsonArray();
        for (Address site : callsites) {
            Instruction instruction = getInstructionAt(site);
            JsonObject row = new JsonObject();
            row.addProperty("address", site.toString());
            row.addProperty("old_flow_override", instruction.getFlowOverride().toString());
            if (instruction.getFlowOverride() == FlowOverride.CALL_RETURN) instruction.setFlowOverride(FlowOverride.NONE);
            Address next = site.add(instruction.getLength());
            row.addProperty("fallthrough_disassembled", disassemble(next));
            restored.add(row);
        }
        JsonArray rebuilt = new JsonArray();
        for (Function caller : callers) {
            JsonObject row = new JsonObject();
            row.addProperty("address", caller.getEntryPoint().toString());
            row.addProperty("old_body_bytes", caller.getBody().getNumAddresses());
            row.addProperty("was_no_return", caller.hasNoReturn());
            caller.setNoReturn(false);
            row.addProperty("body_repaired", CreateFunctionCmd.fixupFunctionBody(currentProgram, caller, monitor));
            row.addProperty("new_body_bytes", caller.getBody().getNumAddresses());
            rebuilt.add(row);
        }
        JsonObject report = new JsonObject();
        report.addProperty("verified_returning_helper", target.toString());
        report.addProperty("evidence", "Exact 11 ARM words checked; both paths reach POP {r4,pc}; no calls or loops.");
        report.addProperty("scope", "Only this accessor, its thunks and directly affected caller flow were repaired; other heuristic findings remain unreviewed.");
        report.add("corrected_targets", corrected); report.add("restored_callsites", restored); report.add("rebuilt_callers", rebuilt);
        Files.writeString(Path.of(args[0]), new GsonBuilder().setPrettyPrinting().create().toJson(report) + "\n");
        println("RETURNING_ACCESSOR_REPAIR_COMPLETE targets="+targets.size()+" calls="+callsites.size()+" callers="+callers.size());
    }
}
