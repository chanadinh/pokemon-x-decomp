/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1085fa8c(undefined4 *param_1,undefined4 *param_2)

{
  *param_1 = *param_2;
  *(undefined2 *)(param_1 + 1) = *(undefined2 *)(param_2 + 1);
  return;
}

