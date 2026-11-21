/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_108acedc(undefined4 param_1,int param_2,int param_3)

{
  return (&DAT_10b11c00)[param_2 * 0x44 + param_3 * 4] != 0;
}

