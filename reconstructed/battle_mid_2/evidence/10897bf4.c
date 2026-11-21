/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10897bf4(int param_1)

{
  *(undefined1 *)(param_1 + 0x24) = 0;
  *(undefined1 *)(param_1 + 0xd5) = 2;
  thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xe0));
  *(undefined1 *)(param_1 + 0xe8) = 0;
  *(undefined1 *)(param_1 + 0xe9) = 3;
  *(undefined1 *)(param_1 + 0xea) = 0x10;
  thunk_FUN_00335dc0(*(undefined4 *)(param_1 + 0xe4),0);
  FUN_108568ec(1);
  FUN_10853d20(*(undefined4 *)(param_1 + 0xd8),0xf);
  return;
}

