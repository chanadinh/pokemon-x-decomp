/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108802a8(int param_1)

{
  *(ushort *)(param_1 + 2) = *(ushort *)(param_1 + 2) | 0x8000;
  return;
}

