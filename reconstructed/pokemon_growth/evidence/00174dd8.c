/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00174dd8(int param_1,int param_2)

{
  if (*(int *)(param_1 + 4) != param_2) {
    FUN_00138214(DAT_005e52b0,param_2,param_1 + 8);
    *(int *)(param_1 + 4) = param_2;
  }
  return;
}

