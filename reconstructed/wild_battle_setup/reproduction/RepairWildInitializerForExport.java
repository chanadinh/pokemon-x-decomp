// Narrow, temporary evidence repair. Run in read-only headless mode.
// @category Nintendo3DS
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.FlowOverride;
import ghidra.program.model.listing.Function;

public class RepairWildInitializerForExport extends GhidraScript {
    public void run() throws Exception {
        if (getInt(toAddr(0x15c75610)) != 0xe92d4010 ||
            getInt(toAddr(0x15c7567c)) != 0xebfe2b79 ||
            getInt(toAddr(0x15c75680)) != 0xe1a00004 ||
            getInt(toAddr(0x15c75684)) != 0xe8bd8010 ||
            getInt(toAddr(0x15c0046c)) != 0x00301a9c) {
            throw new IllegalStateException("Initializer/veneer instruction guard failed");
        }
        for (long address : new long[]{0x00301a9c, 0x15c00468}) {
            Function function = getFunctionAt(toAddr(address));
            if (function == null) throw new IllegalStateException("Missing clear helper");
            function.setNoReturn(false);
        }
        getInstructionAt(toAddr(0x15c7567c)).setFlowOverride(FlowOverride.NONE);
        disassemble(toAddr(0x15c75680));
        getFunctionAt(toAddr(0x15c75610)).setBody(new AddressSet(toAddr(0x15c75610), toAddr(0x15c75687)));
        println("WILD_INITIALIZER_TEMPORARY_REPAIR_COMPLETE size=120");
    }
}
