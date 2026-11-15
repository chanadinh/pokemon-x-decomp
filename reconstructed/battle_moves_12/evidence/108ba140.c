/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ba140(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (*param_4 != 0)) {
    FUN_1084af8c(param_2,param_4[1] & 0xff);
    *param_4 = 0;
  }
  return;
}

