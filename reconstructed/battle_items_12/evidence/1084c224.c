/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084c224(int param_1,uint param_2)

{
  if (param_2 != 0) {
    if (*(ushort *)(param_1 + 0x11c) == param_2) {
      return 0;
    }
    if (*(ushort *)(param_1 + 0x12a) == param_2) {
      return 1;
    }
    if (*(ushort *)(param_1 + 0x138) == param_2) {
      return 2;
    }
    if (*(ushort *)(param_1 + 0x146) == param_2) {
      return 3;
    }
  }
  return 4;
}

