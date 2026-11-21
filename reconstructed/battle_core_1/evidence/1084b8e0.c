/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084b8e0(int param_1,uint param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  iVar2 = 0;
  uVar1 = 0;
  if (*(byte *)(param_1 + 0x108c) != 0) {
    do {
      iVar3 = param_1 + uVar1 * 0x10;
      if ((*(byte *)(*(int *)(iVar3 + 0x10bc) + 0x19) != param_2) &&
         (iVar2 = iVar2 + 1, *(char *)(iVar3 + 0x10c9) != '\0')) {
        iVar4 = iVar4 + 1;
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < *(byte *)(param_1 + 0x108c));
    if (iVar2 != iVar4) {
      return 0;
    }
  }
  return 1;
}

