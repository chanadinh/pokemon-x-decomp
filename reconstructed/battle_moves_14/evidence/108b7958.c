/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7958(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = FUN_108658d4(0x53,1);
  if (iVar1 != 0) {
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,2,0x2ff);
    FUN_1084d29c(iVar1 + 4,param_3);
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

