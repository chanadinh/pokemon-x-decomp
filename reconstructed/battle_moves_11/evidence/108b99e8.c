/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b99e8(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_1086b91c(2);
  if (((iVar1 == param_3) && (iVar1 = FUN_1081cf08(param_2), iVar1 != 0)) &&
     (iVar2 = FUN_108a4724(), iVar2 == 0)) {
    uVar3 = FUN_10871bac(param_2,param_3,iVar1);
    FUN_108658d4(0x12,iVar1);
    FUN_108658d4(0xd,uVar3);
    return;
  }
  return;
}

