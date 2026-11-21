/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10895410(int param_1)

{
  *(undefined1 *)(param_1 + 0x24) = 0;
  *(undefined1 *)(param_1 + 0xe3) = 8;
  thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xfc));
  *(undefined1 *)(param_1 + 0x104) = 0;
  *(undefined1 *)(param_1 + 0x105) = 3;
  *(undefined1 *)(param_1 + 0x106) = 0x88;
  FUN_10850998(param_1);
  thunk_FUN_00334048(*(undefined4 *)(param_1 + 0xfc));
  *(undefined1 *)(param_1 + 0x104) = 1;
  *(undefined1 *)(param_1 + 0x105) = 2;
  *(undefined1 *)(param_1 + 0xe3) = 7;
  *(undefined1 *)(param_1 + 0x106) = 0xf;
  thunk_FUN_0034bcb0(param_1 + 4,*(undefined4 *)(param_1 + 0xf4),0xf,1);
  FUN_108568ec(1);
  return;
}

