/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aba84(int param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(4);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(0x12);
    iVar2 = FUN_108a47f4(uVar1);
    if ((iVar2 != 0) && (iVar2 = FUN_108658d4(0x41,1), iVar2 != 0)) {
      uVar1 = *(undefined2 *)(param_1 + 0x38);
      uVar3 = FUN_1086b91c(0x12);
      iVar2 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar2 + 4,2,0x4ae);
      FUN_1084d29c(iVar2 + 4,param_3);
      FUN_1084d29c(iVar2 + 4,uVar1);
      FUN_1084d29c(iVar2 + 4,uVar3);
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

