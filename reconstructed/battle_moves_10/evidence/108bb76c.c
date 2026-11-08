/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bb76c(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 != param_3) {
    return;
  }
  iVar2 = FUN_1084b5b8(param_2,param_3);
  if (iVar2 != 0) {
    iVar2 = FUN_108544a0(param_2,param_3);
    sVar1 = *(short *)(iVar2 + 0x12);
    iVar2 = FUN_1084cf74(sVar1,7);
    if (sVar1 != 0 && iVar2 != 0) {
      return;
    }
  }
  FUN_108658d4(0x23,0x1b);
  return;
}

