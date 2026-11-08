/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c2850(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if ((iVar1 == param_3) && (iVar1 = FUN_108658d4(0x42,1), iVar1 != 0)) {
    FUN_1084c948(param_2,1,param_3);
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,2,0xd2);
    FUN_1084d29c(iVar1 + 4,param_3);
    FUN_10867b48(param_2,iVar1);
    FUN_1084c948(param_2,2,param_3);
    return;
  }
  return;
}

