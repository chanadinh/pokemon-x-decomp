/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084c96c(int param_1)

{
  if ((*(uint *)(param_1 + 0x18) & 0x1000000) != 0) {
    *(uint *)(param_1 + 0x18) = *(uint *)(param_1 + 0x18) | 0x10000000;
  }
  return;
}

