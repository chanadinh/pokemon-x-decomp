/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a9ad0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(0xc);
  if (iVar1 != 6) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084c140();
    if (iVar1 != 0) {
      FUN_1084c96c(param_1);
      FUN_1084c948(param_2,0,param_3);
      return;
    }
  }
  return;
}

