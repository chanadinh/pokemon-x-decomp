/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10874010(undefined2 *param_1,byte param_2,undefined2 param_3)

{
  *param_1 = param_3;
  *(undefined4 *)(param_1 + 2) = 0;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 6) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  *(undefined4 *)(param_1 + 10) = 0;
  *(undefined4 *)(param_1 + 0xc) = 0;
  *(undefined4 *)(param_1 + 0xe) = 0;
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(byte *)((int)param_1 + 3) = param_2 & 0xf;
  *(undefined1 *)(param_1 + 1) = 0;
  return;
}

