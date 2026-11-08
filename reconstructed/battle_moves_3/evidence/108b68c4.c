/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b68c4(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (1 < *param_4)) {
    FUN_108544a0(param_2,param_3);
    iVar1 = 0;
    if (param_4[1] != 0) {
      iVar1 = param_4[1] << 1;
    }
    FUN_108658d4(0x38,iVar1);
    *param_4 = 3;
  }
  return;
}

