/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10876c28(int param_1,undefined4 param_2)

{
  if ((*(ushort *)(param_1 + 2) & 0x3fff) >> 7 < 8) {
    *(undefined4 *)(param_1 + 0x20) = param_2;
    *(ushort *)(param_1 + 2) = *(ushort *)(param_1 + 2) | 0x4000;
  }
  return;
}

