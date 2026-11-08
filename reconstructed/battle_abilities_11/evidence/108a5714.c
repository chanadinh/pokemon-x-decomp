/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a5714(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  
  uVar1 = FUN_1086b91c(2);
  iVar4 = FUN_10873158(param_2,uVar1);
  if (iVar4 == param_3) {
    uVar5 = FUN_108544a0(param_2,uVar1);
    iVar4 = FUN_1084d318();
    iVar6 = FUN_1086d8ac(uVar5);
    if (iVar6 != 0 || iVar4 == 0) {
      iVar4 = FUN_10869f7c(param_2,0x38,uVar1);
      *(undefined2 *)(iVar4 + 4) = 699;
      *(char *)(iVar4 + 6) = (char)param_3;
      *(undefined1 *)(iVar4 + 7) = 10;
      FUN_1084d2d4(iVar4 + 0xc,2,0x2b9);
      FUN_1084d29c(iVar4 + 0xc,uVar1);
      FUN_10867b48(param_2,iVar4);
      iVar4 = FUN_1084d318(uVar5);
      if (iVar4 == 0) {
        iVar4 = FUN_10869f7c(param_2,4,uVar1);
        *(undefined1 *)(iVar4 + 6) = uVar1;
        sVar2 = FUN_10874420(uVar5,0xe);
        sVar3 = FUN_10874420(uVar5,0xd);
        *(short *)(iVar4 + 4) = sVar2 - sVar3;
        FUN_10867b48(param_2,iVar4);
      }
      iVar4 = FUN_1086d8ac(uVar5);
      if (iVar4 != 0) {
        iVar4 = FUN_10869f7c(param_2,10,uVar1);
        *(undefined1 *)(iVar4 + 0x11) = 1;
        *(undefined1 *)(iVar4 + 5) = uVar1;
        *(undefined1 *)(iVar4 + 4) = 0x26;
        FUN_10867b48(param_2);
      }
    }
    FUN_1086c218(param_2,*(ushort *)(param_1 + 0x38) & 0xff,param_3);
    FUN_1084d1cc(param_1);
    return;
  }
  return;
}

