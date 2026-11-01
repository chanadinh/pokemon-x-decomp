/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_1084ca4c(int param_1,uint param_2)

{
  return (1 << (param_2 & 7) & 0xffU & (uint)*(byte *)(param_1 + 0x169 + (param_2 >> 3))) != 0;
}

