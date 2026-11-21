/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108a4764(ushort *param_1)

{
  ushort uVar1;
  uint uVar2;
  ushort *puVar3;
  
  uVar2 = 0;
  while( true ) {
    puVar3 = &DAT_108d146a + uVar2;
    uVar1 = *puVar3;
    if ((ushort *)(uint)uVar1 != param_1) {
      puVar3 = (ushort *)(uint)(ushort)(&DAT_108d146c)[uVar2];
    }
    if ((ushort *)(uint)uVar1 == param_1 || puVar3 == param_1) break;
    uVar2 = uVar2 + 2;
    if (5 < uVar2) {
      return 0;
    }
  }
  return 1;
}

