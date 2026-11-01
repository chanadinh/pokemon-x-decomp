/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd2dc(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    sVar1 = FUN_1086b91c(0x12);
    iVar2 = thunk_FUN_00168bbc(sVar1,0x1e);
    if (((iVar2 != 0) || (sVar1 == 0x1a)) || (sVar1 == 0x88)) {
      FUN_1084ce84(0x32,0x1333);
      return;
    }
  }
  return;
}

