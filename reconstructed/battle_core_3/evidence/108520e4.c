/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_108520e4(int param_1,uint param_2)

{
  if (param_2 < *(byte *)(param_1 + 0x152)) {
    return *(undefined1 *)(param_1 + param_2 * 0xe + 0x11e);
  }
  FUN_10800394(0,0);
  return 0;
}

