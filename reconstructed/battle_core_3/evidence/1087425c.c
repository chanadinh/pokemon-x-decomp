/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1087425c(uint param_1)

{
  if (param_1 < 6) {
    return 0;
  }
  if ((0xb < param_1) && (param_1 < 0x12)) {
    return 1;
  }
  if ((5 < param_1) && (param_1 < 0xc)) {
    return 2;
  }
  if ((0x11 < param_1) && (param_1 < 0x18)) {
    return 3;
  }
  FUN_10800394(0,0);
  return 0;
}

