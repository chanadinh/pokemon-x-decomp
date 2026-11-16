/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7240(int param_1,undefined4 param_2,int param_3,int *param_4)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar3 = FUN_1086b91c(2);
  if ((iVar3 == param_3) && (iVar3 = FUN_108544a0(param_2,param_3), param_4[6] != 0)) {
    if (*param_4 != 0) {
      *param_4 = *param_4 + -1;
    }
    iVar4 = FUN_1086b91c(0x53);
    if (iVar4 == 0) {
      iVar5 = FUN_10869f7c(param_2,10,param_3);
      *(undefined1 *)(iVar5 + 4) = 0x19;
      *(char *)(iVar5 + 5) = (char)param_3;
      *(undefined1 *)(iVar5 + 0x11) = 1;
      FUN_10867b48(param_2);
    }
    if (*param_4 == 0) {
      iVar4 = FUN_10869f7c(param_2,0xb,param_3);
      *(undefined2 *)(iVar4 + 4) = 6;
      FUN_1084c438(6,iVar3,iVar4 + 8);
      *(char *)(iVar4 + 0xf) = (char)param_3;
      *(undefined1 *)(iVar4 + 0xd) = 1;
      FUN_1084d2d4(iVar4 + 0x14,2,0x168);
      FUN_1084d29c(iVar4 + 0x14,param_3);
      FUN_10867b48(param_2,iVar4);
    }
    else if (iVar4 != 0) {
      return;
    }
    sVar2 = *(short *)(param_1 + 0x38);
    uVar1 = *(undefined1 *)(iVar3 + 0x19);
    iVar3 = FUN_1084d3c0(0,uVar1);
    while (iVar3 != 0) {
      if (*(short *)(iVar3 + 0x38) == sVar2) {
        FUN_1084d1cc();
        iVar3 = FUN_1084d3c0(0,uVar1);
      }
      else {
        iVar3 = FUN_1084d370(iVar3);
      }
    }
  }
  return;
}

