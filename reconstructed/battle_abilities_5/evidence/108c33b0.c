/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c33b0(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084ca4c(uVar2,0xd);
    if ((iVar1 != 0) && (iVar1 = FUN_1086b91c(0x16), iVar1 == 9)) {
      FUN_1084ce84(0x36,0x1800);
      return;
    }
  }
  return;
}

