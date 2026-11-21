/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10882674(int param_1)

{
  *(uint *)(param_1 + 0x18) = *(uint *)(param_1 + 0x18) | 0x4000000;
  return;
}

