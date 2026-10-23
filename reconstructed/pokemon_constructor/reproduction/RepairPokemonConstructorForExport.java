// Guarded, temporary evidence repair; run with headless -readOnly.
// @category Nintendo3DS
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.FlowOverride;
import ghidra.program.model.listing.Function;

public class RepairPokemonConstructorForExport extends GhidraScript {
    public void run() throws Exception {
        if (getInt(toAddr(0x0039a8a4)) != 0xe92d4ff0 ||
            getInt(toAddr(0x0039a968)) != 0x1bfd9c4b ||
            getInt(toAddr(0x0039a974)) != 0xebfd9c48 ||
            getInt(toAddr(0x0039a980)) != 0xebf6d34c ||
            getInt(toAddr(0x0039a98c)) != 0xe8bd8ff0 ||
            getInt(toAddr(0x0039a990)) != 0x000002d2 ||
            getInt(toAddr(0x00301a9c)) != 0xe3a02000 ||
            getInt(toAddr(0x00301aec)) != 0xe12fff1e) {
            throw new IllegalStateException("Constructor instruction/literal guard failed");
        }
        Function clear = getFunctionAt(toAddr(0x00301a9c));
        Function constructor = getFunctionAt(toAddr(0x0039a8a4));
        if (clear == null || constructor == null)
            throw new IllegalStateException("Missing required function");
        clear.setNoReturn(false);
        getInstructionAt(toAddr(0x0039a968)).setFlowOverride(FlowOverride.NONE);
        getInstructionAt(toAddr(0x0039a974)).setFlowOverride(FlowOverride.NONE);
        disassemble(toAddr(0x0039a978));
        constructor.setBody(new AddressSet(toAddr(0x0039a8a4), toAddr(0x0039a98f)));
        println("POKEMON_CONSTRUCTOR_TEMPORARY_REPAIR_COMPLETE size=236");
        runScript("ExportFunctionSet.java", getScriptArgs());
    }
}
