/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4108(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  int iVar1;
  
  if ((*param_4 != 0) &&
     (iVar1 = FUN_108544a0(param_2,param_3,param_3,param_4,param_4),
     *(short *)(iVar1 + 0xc) == 0x1a5)) {
    FUN_1084be58(param_2);
    FUN_1084a944(param_1,param_2,param_3);
  }
  return;
}

