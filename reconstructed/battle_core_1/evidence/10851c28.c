/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_10851c28(int param_1)

{
  return (*(byte *)(param_1 + 0x1b) & 0x20) >> 5;
}

