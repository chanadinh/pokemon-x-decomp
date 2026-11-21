/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10892c34(int param_1)

{
  undefined4 uVar1;
  
  *(undefined1 *)(param_1 + 0x24) = 0;
  *(undefined1 *)(param_1 + 0xd9) = 3;
  thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xec));
  *(undefined1 *)(param_1 + 0xf4) = 0;
  *(undefined1 *)(param_1 + 0xf5) = 3;
  *(undefined1 *)(param_1 + 0xf6) = 0x41;
  FUN_1085168c(param_1);
  uVar1 = thunk_FUN_00497590(*(undefined4 *)(param_1 + 0xe4));
  thunk_FUN_00359404(*(undefined4 *)(param_1 + 0xe4),0);
  thunk_FUN_003594f4(*(undefined4 *)(param_1 + 0xe4));
  thunk_FUN_00359404(*(undefined4 *)(param_1 + 0xe4),uVar1);
  return;
}

