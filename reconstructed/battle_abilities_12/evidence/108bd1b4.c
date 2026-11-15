/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd1b4(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    if (*param_4 == 0) {
      *param_4 = 1;
    }
    else {
      iVar1 = FUN_108658d4(0x23,0x13);
      param_4[1] = iVar1;
      *param_4 = 0;
    }
  }
  return;
}

