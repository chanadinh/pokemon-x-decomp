/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_1084c374(int param_1,uint param_2)

{
  if (param_2 < 0xb) {
    return *(int *)(*(int *)(param_1 + 0x1c) + param_2 * 4 + 0x274) != 0;
  }
  FUN_10800394(0,0);
  return false;
}

