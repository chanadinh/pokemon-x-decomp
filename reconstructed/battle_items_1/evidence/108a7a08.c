/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7a08(int param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    iVar2 = FUN_10869f7c(param_2,4,param_3);
    *(char *)(iVar2 + 6) = (char)param_3;
    uVar1 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    *(undefined2 *)(iVar2 + 4) = uVar1;
    FUN_1084d2d4(iVar2 + 8,2,0x392);
    FUN_1084d29c(iVar2 + 8,param_3);
    FUN_1084d29c(iVar2 + 8,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

