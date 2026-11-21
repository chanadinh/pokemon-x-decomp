/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1086f960(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  
  if (param_2 != 0x12) {
    uVar2 = (uint)*(byte *)(param_1 + 0x100);
    bVar3 = (*(uint *)(param_1 + 0x7c) & 7) != 0;
    if ((uVar2 == 2) && (bVar3)) {
      uVar2 = 0x12;
    }
    if ((*(byte *)(param_1 + 0x101) != 2) || (uVar1 = 0x12, !bVar3)) {
      uVar1 = (uint)*(byte *)(param_1 + 0x101);
    }
    if (uVar2 == 0x12) {
      uVar2 = uVar1;
      if (uVar1 == 0x12) {
        uVar2 = 0;
        uVar1 = uVar2;
      }
    }
    else if (uVar1 == 0x12) {
      uVar1 = uVar2;
    }
    if (uVar2 != param_2 && uVar1 != param_2) {
      param_1 = (uint)*(byte *)(param_1 + 0x102);
    }
    if ((uVar2 == param_2 || uVar1 == param_2) || param_1 == param_2) {
      return 1;
    }
  }
  return 0;
}

