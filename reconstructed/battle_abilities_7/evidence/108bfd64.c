/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bfd64(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  uint *puVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (*param_4 != 0)) {
    puVar2 = (uint *)FUN_10869f7c(param_2,3,param_3);
    *puVar2 = *puVar2 | 0x800000;
    FUN_1084d2d4(puVar2 + 1,2,0xd2);
    FUN_1084d29c(puVar2 + 1,param_3);
    FUN_10867b48(param_2,puVar2);
    *param_4 = 0;
  }
  return;
}

