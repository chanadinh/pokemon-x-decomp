/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1088f210(int param_1,undefined4 param_2)

{
  if (*(int *)(param_1 + 0xe0) != 0) {
    thunk_FUN_00359404();
    FUN_108568ec(param_2);
    return;
  }
  return;
}

