/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4500(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  uint *puVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    *param_4 = 1;
    param_4[1] = 0;
    param_4[2] = 0;
    param_4[3] = 1;
    puVar2 = (uint *)FUN_10869f7c(param_2,3,param_3);
    *puVar2 = *puVar2 | 0x800000;
    FUN_1084d2d4(puVar2 + 1,2,0x1f0);
    FUN_1084d29c(puVar2 + 1,param_3);
    FUN_10867b48(param_2,puVar2);
    return;
  }
  return;
}

