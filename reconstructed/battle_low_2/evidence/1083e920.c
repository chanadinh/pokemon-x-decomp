/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083e920(int param_1,undefined4 param_2,undefined4 param_3)

{
  if (DAT_108fcb84 != 0x97d) {
    DAT_108fcb84 = 0x97d;
  }
  FUN_108a26c8(*(undefined4 *)(param_1 + 400),param_2,0);
  *(undefined4 *)(*(int *)(param_1 + 400) + 0x448) = param_3;
  return;
}

