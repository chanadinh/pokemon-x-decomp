/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bddcc(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x2a), iVar2 == 1)) &&
     (iVar2 = FUN_1086b91c(0x4e), iVar2 == 0)) {
    uVar1 = FUN_1086b91c(0x12);
    iVar2 = FUN_108a4814(uVar1);
    if ((iVar2 == 0) && (iVar2 = thunk_FUN_0039df44(uVar1,1), iVar2 == 0)) {
      FUN_108658d4(0x2b,2);
      return;
    }
  }
  return;
}

