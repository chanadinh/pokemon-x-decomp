/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a790c(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_10874420(uVar2,0xe);
    uVar3 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    iVar4 = FUN_10869f7c(param_2,4,param_3);
    *(char *)(iVar4 + 6) = (char)param_3;
    *(short *)(iVar4 + 4) = (short)((iVar1 * (uVar3 & 0xff)) / 100);
    FUN_1084d2d4(iVar4 + 8,2,0x392);
    FUN_1084d29c(iVar4 + 8,param_3);
    FUN_1084d29c(iVar4 + 8,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar4);
    return;
  }
  return;
}

