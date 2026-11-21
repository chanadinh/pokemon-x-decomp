/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1081d83c(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x1084) + 1;
  *(int *)(param_1 + 0x1084) = iVar1;
  if (iVar1 == 0) {
    iVar1 = 1;
    *(undefined4 *)(param_1 + 0x1084) = 1;
  }
  return iVar1;
}

