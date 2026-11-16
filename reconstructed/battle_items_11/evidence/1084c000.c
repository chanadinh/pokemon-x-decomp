/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_1084c000(int param_1)

{
  bool bVar1;
  
  bVar1 = (*(byte *)(param_1 + 0x1093) & 8) == 0;
  if (bVar1) {
    *(byte *)(param_1 + 0x1093) = *(byte *)(param_1 + 0x1093) | 8;
  }
  return bVar1;
}

