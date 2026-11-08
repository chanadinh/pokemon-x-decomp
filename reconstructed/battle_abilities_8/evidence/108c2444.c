/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c2444(undefined4 param_1,undefined4 param_2,int param_3,uint *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    iVar1 = FUN_108544a0(param_2,param_3);
    *param_4 = (uint)(*(short *)(iVar1 + 0x12) != 0);
  }
  return;
}

