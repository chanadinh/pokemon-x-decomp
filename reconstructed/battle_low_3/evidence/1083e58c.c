/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083e58c(int param_1,undefined1 param_2,undefined4 param_3,undefined4 param_4,
                 undefined1 param_5,undefined1 param_6)

{
  undefined2 uVar1;
  
  if (DAT_108fcb84 != 0x1711) {
    DAT_108fcb84 = 0x1711;
  }
  FUN_1086fd18((undefined1 *)(param_1 + 0x6ca),0xff);
  *(undefined1 *)(param_1 + 0x25d) = 0;
  *(undefined1 *)(param_1 + 0x6c8) = 0;
  *(undefined1 *)(param_1 + 0x6c9) = param_2;
  *(undefined1 *)(param_1 + 0x6ca) = param_2;
  *(undefined4 *)(param_1 + 0x6d0) = 0;
  *(undefined4 *)(param_1 + 0x6d4) = 0;
  uVar1 = thunk_FUN_0039f108(param_3);
  *(undefined2 *)(param_1 + 0x6e4) = uVar1;
  *(undefined1 *)(param_1 + 0x25d) = 0;
  *(undefined4 *)(param_1 + 0x6b8) = 0;
  *(undefined4 *)(param_1 + 0x6bc) = 0;
  *(undefined4 *)(param_1 + 0x6fc) = param_3;
  *(undefined4 *)(param_1 + 0x700) = param_4;
  *(undefined1 *)(param_1 + 0x71c) = param_5;
  *(undefined1 *)(param_1 + 0x71d) = param_6;
  return;
}

