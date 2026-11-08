/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aafe8(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x47), iVar1 == 0)) &&
     (iVar1 = FUN_1086b91c(0x39), 7 < iVar1)) {
    FUN_1084c96c(param_1);
    iVar1 = FUN_1086b91c(3);
    *param_4 = iVar1 + 1;
    iVar1 = FUN_10869f7c(param_2,0,param_3);
    *(uint *)(iVar1 + 4) = *(uint *)(iVar1 + 4) | 2;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

