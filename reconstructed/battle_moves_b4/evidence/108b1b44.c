/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b1b44(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    uVar4 = FUN_1084d2f4(uVar3,2);
    iVar2 = FUN_1084b82c(uVar3,1);
    uVar5 = FUN_10874420(uVar3,0xd);
    if ((uVar4 < uVar5) && (iVar2 != 0)) {
      iVar6 = FUN_10869f7c(param_2,7,param_3);
      *(undefined1 *)(iVar6 + 4) = 1;
      *(uint *)(iVar6 + 0x14) = -uVar4;
      *(undefined1 *)(iVar6 + 6) = 1;
      uVar1 = (undefined1)param_3;
      *(undefined1 *)(iVar6 + 7) = uVar1;
      FUN_10867b48(param_2);
      iVar6 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar6 + 4) = 1;
      *(undefined1 *)(iVar6 + 0xc) = uVar1;
      *(char *)(iVar6 + 0x17) = (char)iVar2;
      *(undefined1 *)(iVar6 + 0x16) = 1;
      *(byte *)(iVar6 + 0x18) = *(byte *)(iVar6 + 0x18) | 7;
      FUN_10867b48(param_2);
      iVar2 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar2 + 4,2,0x265);
      FUN_1084d29c(iVar2 + 4,param_3);
      FUN_10867b48(param_2,iVar2);
      iVar2 = FUN_10869f7c(param_2,0x22,param_3);
      *(undefined1 *)(iVar2 + 4) = uVar1;
      *(undefined1 *)(iVar2 + 5) = 1;
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

