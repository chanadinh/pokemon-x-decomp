/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a55e0(int param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  iVar1 = FUN_1086b91c(2);
  if (((iVar1 == 0x1f) && (iVar1 = FUN_10865c48(param_2), iVar1 == 0)) &&
     (uVar2 = FUN_1084d364(param_2), *param_4 <= (int)(uVar2 & 0xff))) {
    uVar2 = FUN_1084d348(param_2,param_3);
    uVar3 = param_4[6];
    uVar4 = uVar3 & 0xff;
    if ((uVar2 != 0x1f) && (uVar2 != uVar4)) {
      FUN_108544a0(param_2,uVar2);
      iVar1 = FUN_1085f844();
      if (iVar1 == 0) {
        iVar1 = FUN_10869f7c(param_2,3,uVar4);
        FUN_1084d2d4(iVar1 + 4,2,0x43e);
        FUN_1084d29c(iVar1 + 4,uVar2);
        FUN_1084d29c(iVar1 + 4,param_4[1]);
        FUN_10867b48(param_2,iVar1);
        iVar1 = FUN_10869f7c(param_2,0x28,uVar4);
        *(char *)(iVar1 + 4) = (char)uVar3;
        *(char *)(iVar1 + 5) = (char)uVar2;
        *(char *)(iVar1 + 6) = (char)param_4[2];
        *(short *)(iVar1 + 8) = (short)param_4[1];
        FUN_10867b48(param_2);
      }
    }
    FUN_1086c218(param_2,*(ushort *)(param_1 + 0x38) & 0xff,param_3);
    FUN_1084d1cc(param_1);
    return;
  }
  return;
}

