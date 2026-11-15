/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aa6dc(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_1086b91c(0x2f);
  if (((uVar1 & 8) != 0) && (iVar2 = FUN_1086b91c(2), iVar2 == param_3)) {
    FUN_108544a0(param_2,param_3);
    iVar2 = FUN_1084c140();
    if (iVar2 != 0) {
      FUN_1084c96c(param_1);
      FUN_1084c948(param_2,0,param_3);
      return;
    }
  }
  return;
}

