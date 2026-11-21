/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bb084(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    FUN_1084cb78(param_2,param_3);
    uVar2 = FUN_1084c3b0();
    FUN_108658d4(0x1f,uVar2);
    return;
  }
  return;
}

