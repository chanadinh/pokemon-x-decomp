/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108c8548(int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  
  uVar1 = param_1 + 0x100;
  uVar2 = (uint)*(ushort *)(param_1 + 0x11c);
  bVar3 = uVar2 != param_2;
  if (bVar3) {
    uVar2 = (uint)*(ushort *)(param_1 + 0x12a);
  }
  if (bVar3 && uVar2 != param_2) {
    bVar3 = *(ushort *)(param_1 + 0x138) != param_2;
    if (bVar3) {
      uVar1 = (uint)*(ushort *)(param_1 + 0x146);
    }
    if (bVar3 && uVar1 != param_2) {
      return 0;
    }
  }
  return 1;
}

