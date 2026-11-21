/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b3e78(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uStack_18;
  
  uStack_18 = param_4;
  iVar2 = FUN_1084b784(param_2,param_3,&uStack_18);
  if (iVar2 != 0) {
    uVar1 = FUN_1086b91c(6);
    iVar2 = FUN_1084c5a8(uVar1,param_3);
    if (iVar2 == 0) {
      FUN_10873158(param_2,uVar1);
      FUN_10873158(param_2,param_3);
      iVar2 = FUN_1084b67c(param_2,param_3,uVar1,0);
      if (iVar2 == 0) {
        FUN_1081d748(param_2,param_3);
      }
    }
  }
  return;
}

