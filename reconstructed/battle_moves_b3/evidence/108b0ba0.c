/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b0ba0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  ushort uVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar1 = FUN_1086b91c(6);
    uVar4 = FUN_108544a0(param_2,uVar1);
    uVar2 = FUN_10874420(uVar4,0x10);
    iVar3 = FUN_108a48e4(uVar2 & 0xff);
    if (iVar3 == 0) {
      iVar3 = FUN_10869f7c(param_2,0x20,param_3);
      *(char *)(iVar3 + 6) = (char)param_3;
      *(ushort *)(iVar3 + 4) = uVar2 & 0xff;
      FUN_1084d2d4(iVar3 + 0xc,2,0x26b);
      FUN_1084d29c(iVar3 + 0xc,param_3);
      FUN_1084d29c(iVar3 + 0xc,uVar1);
      FUN_1084d29c(iVar3 + 0xc,*(undefined2 *)(iVar3 + 4));
      FUN_10867b48(param_2,iVar3);
      return;
    }
  }
  return;
}

