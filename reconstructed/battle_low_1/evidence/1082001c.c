/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1082001c(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  iVar1 = FUN_1087425c(param_2);
  uVar3 = iVar1 + 2U & 3;
  iVar1 = FUN_10870368(*(undefined4 *)(param_1 + 4),uVar3);
  if (iVar1 == 0) {
    uVar3 = 4;
  }
  iVar2 = FUN_10870368(*(undefined4 *)(param_1 + 4),uVar3);
  iVar1 = 0;
  if (iVar2 != 0) {
    iVar1 = *(int *)(param_1 + 8);
    if (3 < uVar3) {
      FUN_10800394(0,0);
    }
    iVar1 = iVar1 + uVar3 * 0x1c + 4;
  }
  return iVar1;
}

