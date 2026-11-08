/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108be2c0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  uVar1 = FUN_1086b91c(3);
  iVar2 = FUN_1084c5a8(param_3,uVar1);
  if (iVar2 != 0) {
    FUN_1084ce84(0x36,0x119a);
    return;
  }
  return;
}

