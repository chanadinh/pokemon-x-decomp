/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c1728(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(0x12);
    iVar2 = thunk_FUN_0039ddc0(uVar1);
    if (iVar2 == 0) {
      iVar2 = FUN_1086b91c(0x19);
      FUN_108658d4(0x19,iVar2 + 1);
      return;
    }
  }
  return;
}

