/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a9710(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    uVar3 = FUN_1084c278();
    iVar1 = FUN_1084c224(uVar2,uVar3);
    if ((iVar1 != 4) && (iVar1 = FUN_108520e4(uVar2,iVar1), iVar1 == 0)) {
      FUN_108658d4(0x53,1);
      return;
    }
  }
  return;
}

