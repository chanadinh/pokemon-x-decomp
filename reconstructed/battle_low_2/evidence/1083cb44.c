/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1083cb44(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  if (DAT_108fcb84 != 0x1057) {
    DAT_108fcb84 = 0x1057;
  }
  if (*(int *)(param_1 + 0x6b8) == 0xff) {
    uVar1 = 1;
  }
  else {
    iVar2 = FUN_10858cb8(param_1,0);
    uVar1 = 0;
    if (iVar2 != 0) {
      FUN_1085db0c(param_1,0);
      FUN_10859cc4(*(int *)(param_1 + 400) + 0x280);
      uVar1 = 1;
    }
  }
  return uVar1;
}

