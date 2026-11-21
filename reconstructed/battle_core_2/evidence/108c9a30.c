/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_108c9a30(int param_1,uint param_2,uint param_3)

{
  if ((param_2 < 4) && (param_3 < *(byte *)(param_1 + param_2 * 0x31))) {
    return *(undefined1 *)(param_1 + param_2 * 0x31 + param_3 + 0x19);
  }
  return 0x1f;
}

