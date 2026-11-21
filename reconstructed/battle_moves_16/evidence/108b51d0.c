/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b51d0(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(3);
  if ((iVar3 == param_3) && (*param_4 != 0)) {
    uVar1 = FUN_1086b91c(4);
    uVar4 = FUN_108544a0(param_2,uVar1);
    iVar3 = FUN_1084d318();
    if (iVar3 != 0) {
      iVar3 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar3 + 4,2,0xd2);
      FUN_1084d29c(iVar3 + 4,uVar1);
      FUN_10867b48(param_2,iVar3);
      return;
    }
    iVar3 = FUN_10869f7c(param_2,4,param_3);
    *(undefined1 *)(iVar3 + 6) = uVar1;
    uVar2 = FUN_1084d2f4(uVar4,4);
    *(undefined2 *)(iVar3 + 4) = uVar2;
    FUN_1084d2d4(iVar3 + 8,2,0x183);
    FUN_1084d29c(iVar3 + 8,uVar1);
    FUN_10867b48(param_2,iVar3);
    return;
  }
  return;
}

