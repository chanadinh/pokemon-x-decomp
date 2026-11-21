/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1083bdd4(int param_1)

{
  if (DAT_108fcb84 != 0xdf9) {
    DAT_108fcb84 = 0xdf9;
  }
  FUN_10859cc4(*(int *)(param_1 + 400) + 0x280);
  return 1;
}

