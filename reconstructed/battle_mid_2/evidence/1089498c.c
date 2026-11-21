/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1089498c(int param_1)

{
  *(undefined1 *)(param_1 + 0x24) = 0;
  *(undefined1 *)(param_1 + 0xd9) = 3;
  thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xec));
  *(undefined1 *)(param_1 + 0xf4) = 0;
  *(undefined1 *)(param_1 + 0xf5) = 3;
  *(undefined1 *)(param_1 + 0xf6) = 0x5f;
  FUN_10850cf4(param_1);
  thunk_FUN_00334048(*(undefined4 *)(param_1 + 0xec));
  *(undefined1 *)(param_1 + 0xf4) = 1;
  *(undefined1 *)(param_1 + 0xf5) = 2;
  *(undefined1 *)(param_1 + 0xd9) = 2;
  return;
}

