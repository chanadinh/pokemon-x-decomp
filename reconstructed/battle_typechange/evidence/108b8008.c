/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8008(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    FUN_108544a0(param_2,param_3);
    uVar2 = FUN_1089e61c();
    FUN_108658d4(0x1c,uVar2);
    return;
  }
  return;
}

