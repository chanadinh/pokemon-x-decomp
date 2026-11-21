/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1089289c(int param_1)

{
  *(undefined1 *)(param_1 + 0x24) = 0;
  *(undefined1 *)(param_1 + 0xd9) = 3;
  thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xec));
  *(undefined1 *)(param_1 + 0xf4) = 0;
  *(undefined1 *)(param_1 + 0xf5) = 3;
  *(undefined1 *)(param_1 + 0xf6) = 0x41;
  FUN_1085168c(param_1);
  *(undefined1 *)(param_1 + 0xd9) = 2;
  thunk_FUN_00334048(*(undefined4 *)(param_1 + 0xec));
  *(undefined1 *)(param_1 + 0xf4) = 1;
  *(undefined1 *)(param_1 + 0xf5) = 2;
  return;
}

