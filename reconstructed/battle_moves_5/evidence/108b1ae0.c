/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b1ae0(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    FUN_1084c3cc(param_2,param_3,1);
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,1,0xff);
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

