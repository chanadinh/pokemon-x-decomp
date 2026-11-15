/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c16a0(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084aa9c();
    if (iVar1 != 0) {
      iVar1 = FUN_10869f7c(param_2,0x35,param_3);
      *(char *)(iVar1 + 4) = (char)param_3;
      FUN_1084d2d4(iVar1 + 8,2,0x1de);
      FUN_1084d29c(iVar1 + 8,param_3);
      FUN_10867b48(param_2,iVar1);
      return;
    }
  }
  return;
}

