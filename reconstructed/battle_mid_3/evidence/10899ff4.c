/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10899ff4(int param_1,undefined4 param_2)

{
  if (*(int *)(param_1 + 0xdc) != 0) {
    thunk_FUN_00359404();
    FUN_108568ec(param_2);
    return;
  }
  return;
}

