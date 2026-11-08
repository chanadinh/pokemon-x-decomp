/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bbd78(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(6);
    uVar3 = FUN_108544a0(param_2,uVar1);
    iVar2 = FUN_10856a88(uVar3,2);
    if (iVar2 != 0) {
      iVar2 = FUN_10869f7c(param_2,10,param_3);
      *(undefined1 *)(iVar2 + 0x11) = 1;
      *(undefined1 *)(iVar2 + 5) = uVar1;
      *(undefined1 *)(iVar2 + 4) = 2;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

