/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c503c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  uint *puVar1;
  
  if (*param_4 == 0) {
    *param_4 = 1;
    param_4[1] = 0;
    param_4[2] = 0;
    param_4[3] = 1;
    puVar1 = (uint *)FUN_10869f7c(param_2,3);
    *puVar1 = *puVar1 | 0x800000;
    FUN_1084d2d4(puVar1 + 1,2,0x1f0);
    FUN_1084d29c(puVar1 + 1,param_3);
    FUN_10867b48(param_2,puVar1);
    return;
  }
  return;
}

