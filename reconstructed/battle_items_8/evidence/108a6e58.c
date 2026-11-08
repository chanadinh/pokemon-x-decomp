/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6e58(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(4);
  if ((iVar3 == param_3) && (iVar3 = FUN_1086b91c(0x47), iVar3 == 0)) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar2,0);
    if (iVar3 != 0) {
      iVar3 = FUN_10869f7c(param_2,6,param_3);
      uVar1 = FUN_1086b91c(3);
      *(undefined1 *)(iVar3 + 6) = uVar1;
      uVar4 = FUN_108544a0(param_2);
      uVar1 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
      uVar2 = FUN_1084d2f4(uVar4,uVar1);
      *(undefined2 *)(iVar3 + 4) = uVar2;
      FUN_1084d2d4(iVar3 + 0xc,2,0x1a8);
      FUN_1084d29c(iVar3 + 0xc,*(undefined1 *)(iVar3 + 6));
      FUN_10867b48(param_2,iVar3);
      return;
    }
  }
  return;
}

