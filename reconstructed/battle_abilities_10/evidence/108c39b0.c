/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c39b0(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  if (((*param_4 != 0) && (iVar1 = FUN_1086b91c(2), iVar1 == param_3)) &&
     (iVar1 = FUN_108544a0(param_2,param_3), *(short *)(iVar1 + 0xc) == 0x1a5)) {
    FUN_1084a944(param_1,param_2,param_3,0,0);
  }
  return;
}

