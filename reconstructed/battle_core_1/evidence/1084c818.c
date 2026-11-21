/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_1084c818(int param_1,uint param_2)

{
  return (1 << (param_2 & 7) & 0xffU & (uint)*(byte *)(param_1 + 0x166 + (param_2 >> 3))) != 0;
}

