/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b59a8(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    sVar1 = FUN_1084bb2c(uVar3,0);
    FUN_108658d4(0x31,sVar1 * 100);
    return;
  }
  return;
}

