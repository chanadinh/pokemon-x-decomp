/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086b284(uint *param_1)

{
  *param_1 = *param_1 & 0x8007fc00 | ((*param_1 & 0x7fc00) >> 10) << 0x13;
  return;
}

