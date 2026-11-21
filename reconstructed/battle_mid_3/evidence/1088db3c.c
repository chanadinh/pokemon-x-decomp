/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1088db3c(int param_1,undefined4 param_2)

{
  if (*(int *)(param_1 + 0xd8) != 0) {
    thunk_FUN_00359404();
    FUN_108568ec(param_2);
    return;
  }
  return;
}

