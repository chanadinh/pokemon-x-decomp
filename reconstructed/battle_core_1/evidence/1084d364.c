/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1084d364(int param_1)

{
  return *(uint *)(param_1 + 0x14) & 0xffff;
}

