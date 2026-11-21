/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b0a10(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    iVar2 = FUN_10851c28();
    if (iVar2 == 0) {
      uVar1 = FUN_1086b91c(6);
      uVar4 = FUN_108544a0(param_2,uVar1);
      iVar2 = FUN_1084c7fc();
      iVar5 = FUN_1084c278(uVar4);
      if (iVar5 != iVar2) {
        iVar2 = iVar5;
      }
      if ((((iVar2 != 0) && (iVar5 = FUN_108a4844(iVar2), iVar5 == 0)) &&
          (iVar5 = FUN_1084c224(uVar3,iVar2), iVar5 == 4)) &&
         (iVar5 = FUN_1084c224(uVar3,*(undefined2 *)(param_1 + 0x38)), iVar5 != 4)) {
        iVar6 = FUN_10869f7c(param_2,0x26,param_3);
        *(char *)(iVar6 + 5) = (char)iVar5;
        *(undefined1 *)(iVar6 + 6) = 0;
        *(char *)(iVar6 + 4) = (char)param_3;
        *(undefined1 *)(iVar6 + 7) = 0;
        *(short *)(iVar6 + 8) = (short)iVar2;
        FUN_10867b48(param_2,iVar6);
        iVar5 = FUN_10869f7c(param_2,3,param_3);
        FUN_1084d2d4(iVar5 + 4,2,0x2b0);
        FUN_1084d29c(iVar5 + 4,param_3);
        FUN_1084d29c(iVar5 + 4,iVar2);
        FUN_10867b48(param_2,iVar5);
        return;
      }
    }
  }
  return;
}

