/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1084beac(uint param_1,int param_2)

{
  return (param_1 & 0x3f) << 3 | 2 | param_2 << 9;
}

