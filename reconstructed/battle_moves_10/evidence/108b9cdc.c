/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b9cdc(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    uVar3 = FUN_1084bb2c(uVar2,0);
    if (2 < uVar3) {
      FUN_108658d4(0x23,0x1b);
      return;
    }
  }
  return;
}

