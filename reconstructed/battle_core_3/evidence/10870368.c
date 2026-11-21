/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10870368(int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  
  if (param_2 < 4) {
    uVar1 = 0;
    do {
      uVar2 = param_1 + uVar1;
      bVar3 = *(byte *)(uVar2 + 0x7ec) != param_2;
      if (bVar3) {
        uVar2 = (uint)*(byte *)(uVar2 + 0x7ed);
      }
      if (!bVar3 || uVar2 == param_2) {
        return 1;
      }
      uVar1 = uVar1 + 2;
    } while (uVar1 < 10);
  }
  return 0;
}

