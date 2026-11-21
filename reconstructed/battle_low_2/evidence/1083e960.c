/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083e960(int param_1,undefined4 param_2)

{
  if (DAT_108fcb84 != 0x969) {
    DAT_108fcb84 = 0x969;
  }
  FUN_10858150(*(int *)(param_1 + 400) + 0x3a0,*(undefined4 *)(*(int *)(param_1 + 400) + 0x7b0),0,
               param_2,0,0);
  return;
}

