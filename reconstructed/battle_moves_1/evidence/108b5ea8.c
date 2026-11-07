/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5ea8(int param_1,undefined4 param_2,int param_3,uint *param_4)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    uVar4 = *param_4;
    iVar2 = 1;
    if (uVar4 != 0) {
      if ((uVar4 & 1) != 0) {
        iVar2 = 2;
      }
      for (uVar4 = uVar4 >> 1; uVar4 != 0; uVar4 = uVar4 - 1) {
        iVar2 = iVar2 << 2;
      }
    }
    iVar5 = FUN_1084ca4c(uVar3,7);
    if (iVar5 != 0) {
      iVar2 = iVar2 << 1;
    }
    iVar5 = FUN_1086b91c(0x31);
    FUN_108658d4(0x31,iVar5 * iVar2);
    uVar4 = *param_4;
    *param_4 = uVar4 + 1;
    if ((int)param_4[1] <= (int)(uVar4 + 1)) {
      sVar1 = *(short *)(param_1 + 0x38);
      iVar2 = FUN_1084d3c0(0,param_3);
      while (iVar2 != 0) {
        if (*(short *)(iVar2 + 0x38) == sVar1) {
          FUN_1084d1cc();
          iVar2 = FUN_1084d3c0(0,param_3);
        }
        else {
          iVar2 = FUN_1084d370(iVar2);
        }
      }
    }
  }
  return;
}

