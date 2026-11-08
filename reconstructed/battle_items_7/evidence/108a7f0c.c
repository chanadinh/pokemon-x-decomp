/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7f0c(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x4e), iVar1 == 0)) &&
     (iVar1 = FUN_1086b91c(0x33), iVar1 != 0)) {
    uVar2 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    iVar1 = thunk_FUN_0015e958(iVar1,uVar2);
    if (iVar1 == 0) {
      iVar1 = 1;
    }
    iVar3 = FUN_10869f7c(param_2,4,param_3);
    *(char *)(iVar3 + 6) = (char)param_3;
    *(short *)(iVar3 + 4) = (short)iVar1;
    FUN_1084d2d4(iVar3 + 8,2,0x398);
    FUN_1084d29c(iVar3 + 8,param_3);
    FUN_1084d29c(iVar3 + 8,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar3);
    return;
  }
  return;
}

