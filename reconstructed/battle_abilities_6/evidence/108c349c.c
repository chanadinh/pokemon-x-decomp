/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c349c(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if ((iVar2 == param_3) && (sVar1 = FUN_1086b91c(0x1e), sVar1 == 4)) {
    iVar2 = FUN_1086b91c(0x33);
    uVar3 = FUN_1089e6fc(iVar2 / 2,1);
    FUN_108658d4(0x33,uVar3);
    return;
  }
  return;
}

