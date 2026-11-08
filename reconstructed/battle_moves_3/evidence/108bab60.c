/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bab60(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    uVar2 = FUN_10874420(uVar2,0xd);
    FUN_108658d4(0x38,uVar2);
    return;
  }
  return;
}

