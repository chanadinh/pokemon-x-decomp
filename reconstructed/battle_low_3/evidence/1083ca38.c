/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083ca38(int param_1,undefined4 param_2,undefined4 param_3,undefined1 param_4,
                 undefined4 param_5)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0xa18) {
    DAT_108fcb84 = 0xa18;
  }
  *(undefined4 *)(param_1 + 0x760) = param_2;
  *(undefined4 *)(param_1 + 0x764) = param_5;
  iVar1 = *(int *)(param_1 + 400);
  FUN_10856954(iVar1);
  *(undefined1 *)(iVar1 + 8) = 0;
  *(undefined1 *)(iVar1 + 0x1c9) = 0;
  *(undefined4 *)(iVar1 + 0x1c4) = param_2;
  *(undefined1 *)(iVar1 + 0x1c8) = param_4;
  *(undefined1 *)(iVar1 + 0x1cc) = 1;
  *(undefined4 *)(iVar1 + 0x1d0) = param_3;
  FUN_1084a214(iVar1 + 0x140,iVar1 + 0x1c4);
  FUN_1084a0bc(iVar1 + 0x140);
  FUN_108577b4(*(undefined4 *)(param_1 + 400));
  return;
}

