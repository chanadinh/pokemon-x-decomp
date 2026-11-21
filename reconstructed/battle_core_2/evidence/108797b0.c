/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_108797b0(uint param_1,uint param_2)

{
  return (param_1 & 0x3f) << 9 | (param_2 & 0x3f) << 3 | 4;
}

