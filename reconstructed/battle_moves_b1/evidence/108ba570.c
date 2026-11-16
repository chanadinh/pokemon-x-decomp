/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ba570(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(4);
  if (iVar3 == param_3) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar2,0);
    if (iVar3 != 0) {
      uVar1 = FUN_1086b91c(3);
      uVar4 = FUN_108544a0(param_2,uVar1);
      iVar3 = FUN_10869f7c(param_2,6,param_3);
      *(undefined1 *)(iVar3 + 6) = uVar1;
      uVar2 = FUN_1084d2f4(uVar4,8);
      *(undefined2 *)(iVar3 + 4) = uVar2;
      *(byte *)(iVar3 + 7) = *(byte *)(iVar3 + 7) & 0xfe;
      FUN_1084d2d4(iVar3 + 0xc,2,0x4c0);
      FUN_1084d29c(iVar3 + 0xc,uVar1);
      FUN_10867b48(param_2,iVar3);
      return;
    }
  }
  return;
}

