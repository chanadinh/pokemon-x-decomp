/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8d7c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1086d8ac();
    if ((iVar1 == 5 || iVar1 == 1) || iVar1 == 4) {
      FUN_1084ce84(0x32,0x2000);
      return;
    }
  }
  return;
}

