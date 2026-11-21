/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108395d8(int param_1,undefined4 param_2)

{
  if (DAT_108fcb84 != 0xfb3) {
    DAT_108fcb84 = 0xfb3;
  }
  FUN_1086fd18((undefined1 *)(param_1 + 0x6ca),0xff);
  *(undefined1 *)(param_1 + 0x25d) = 0;
  *(undefined1 *)(param_1 + 0x6c8) = 0;
  *(undefined1 *)(param_1 + 0x6c9) = 0;
  *(undefined1 *)(param_1 + 0x6ca) = 0;
  *(undefined4 *)(param_1 + 0x6b8) = 0;
  *(undefined4 *)(param_1 + 0x6e0) = param_2;
  FUN_10859464(param_1,param_2);
  return;
}

