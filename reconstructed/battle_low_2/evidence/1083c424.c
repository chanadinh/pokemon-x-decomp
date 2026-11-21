/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083c424(int param_1)

{
  if (DAT_108fcb84 != 0x11a6) {
    DAT_108fcb84 = 0x11a6;
  }
  if (*(int *)(param_1 + 0x6b8) == 0) {
    FUN_10858cb8(param_1,0);
    return;
  }
  return;
}

