/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b57f8(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x47), iVar1 == 0)) &&
     (*(int *)(param_4 + 0x18) != 0)) {
    iVar1 = FUN_1086b91c(0x33);
    *(int *)(param_4 + 4) = iVar1 + *(int *)(param_4 + 4);
  }
  return;
}

