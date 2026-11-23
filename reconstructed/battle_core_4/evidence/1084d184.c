/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

char FUN_1084d184(int param_1,uint param_2)

{
  if (*(byte *)(param_1 + 0x152) <= param_2) {
    FUN_10800394(0,0);
  }
  param_1 = param_1 + param_2 * 0xe;
  return *(char *)(param_1 + 0x11f) - *(char *)(param_1 + 0x11e);
}

