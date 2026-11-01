/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bfc9c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_10856a88(uVar2,5);
    if ((iVar1 != 0) && (iVar1 = FUN_1086b91c(0x1b), iVar1 == 1)) {
      FUN_1084ce84(0x32,0x1800);
      return;
    }
  }
  return;
}

