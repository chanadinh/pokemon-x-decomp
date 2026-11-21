/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1087c024(uint param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  bool bVar4;
  
  uVar3 = 0;
  for (iVar1 = DAT_108fca60; iVar1 != 0; iVar1 = *(int *)(iVar1 + 4)) {
    uVar2 = (uint)*(byte *)(iVar1 + 0x3b);
    bVar4 = uVar2 == param_1;
    if (bVar4) {
      uVar2 = (uint)*(byte *)(iVar1 + 0x10);
    }
    if (bVar4 && uVar2 == param_2) {
      uVar3 = 1;
      *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) & 0xbfffffff;
    }
  }
  return uVar3;
}

