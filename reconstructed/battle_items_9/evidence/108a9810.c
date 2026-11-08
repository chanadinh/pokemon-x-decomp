/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a9810(int param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (*param_4 != 0)) {
    iVar1 = FUN_10869f7c(param_2,0x24,param_3);
    FUN_1084d2d4(iVar1 + 8,2,0x3f5);
    FUN_1084d29c(iVar1 + 8,param_3);
    FUN_1084d29c(iVar1 + 8,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar1);
    *param_4 = 0;
  }
  return;
}

