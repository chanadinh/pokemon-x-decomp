/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aaa48(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (*param_4 != 0)) {
    FUN_1084c96c(param_1);
    FUN_1084c948(param_2,0,param_3);
    *param_4 = 0;
  }
  return;
}

