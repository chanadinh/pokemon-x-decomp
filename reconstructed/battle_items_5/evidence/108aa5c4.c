/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aa5c4(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(0x12);
    iVar2 = thunk_FUN_0039ddc0(uVar1);
    if (iVar2 == 2) {
      FUN_1084ce84(0x36,0x1800);
      return;
    }
  }
  return;
}

