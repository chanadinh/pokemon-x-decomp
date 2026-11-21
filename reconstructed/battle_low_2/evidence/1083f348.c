/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083f348(int param_1,undefined1 param_2,undefined1 param_3)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0xa44) {
    DAT_108fcb84 = 0xa44;
  }
  *(undefined4 *)(param_1 + 0x764) = 0;
  iVar1 = *(int *)(param_1 + 400);
  FUN_10856954(iVar1);
  *(undefined1 *)(iVar1 + 8) = 0;
  *(undefined1 *)(iVar1 + 0x1c9) = 1;
  *(undefined1 *)(iVar1 + 0x1ca) = param_2;
  *(undefined1 *)(iVar1 + 0x1cb) = param_3;
  *(undefined1 *)(iVar1 + 0x1cc) = 1;
  FUN_1084a214(iVar1 + 0x140,iVar1 + 0x1c4);
  return;
}

