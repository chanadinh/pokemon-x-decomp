/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ae96c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(4);
    uVar3 = FUN_108544a0(param_2,uVar1);
    iVar2 = FUN_10856a88(uVar3,1);
    if (iVar2 != 0) {
      iVar2 = FUN_1086b91c(0x31);
      FUN_108658d4(0x31,iVar2 << 1);
      return;
    }
  }
  return;
}

