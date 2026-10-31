/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084cf8c(int param_1,uint param_2,undefined4 param_3)

{
  if (6 < param_2) {
    FUN_10800394(0,0);
  }
  *(undefined4 *)(param_1 + param_2 * 4 + 0x1c) = param_3;
  return;
}

