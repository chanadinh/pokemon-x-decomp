/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b27d4(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_1086b91c(0xd);
    uVar2 = uVar2 & 0xff;
    iVar1 = FUN_1086b91c(3);
    if ((iVar1 == param_3) && (iVar1 = FUN_108658d4(0x53,1), iVar1 != 0)) {
      uVar3 = FUN_10873158(param_2,param_3);
      if (uVar2 == 10) {
        uVar4 = FUN_1084bf14(param_2);
        uVar2 = FUN_1086de1c(uVar4,uVar3,0);
        FUN_108658d4(0xd,uVar2);
      }
      iVar1 = FUN_10869f7c(param_2,0x1f,param_3);
      *(char *)(iVar1 + 5) = (char)uVar2;
      *(undefined1 *)(iVar1 + 4) = 3;
      iVar5 = FUN_1084d364(param_2);
      *(int *)(iVar1 + 8) = iVar5 + 2;
      *(uint *)(iVar1 + 0xc) = (uint)*(ushort *)(param_1 + 0x38);
      *(undefined1 *)(iVar1 + 0x18) = 3;
      *(undefined4 *)(iVar1 + 0x10) = uVar3;
      FUN_10867b48(param_2,iVar1);
      return;
    }
  }
  return;
}

