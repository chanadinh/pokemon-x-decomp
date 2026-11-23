/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1083b470(int param_1)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x9bc) {
    DAT_108fcb84 = 0x9bc;
  }
  if (*(int *)(param_1 + 0xf0) == 0) {
    iVar1 = FUN_108a2abc(*(undefined4 *)(param_1 + 400),*(undefined4 *)(param_1 + 0x764));
    if (iVar1 == 0) {
      return 0;
    }
    FUN_10836230(*(int *)(param_1 + 400) + 0xa8);
    *(int *)(param_1 + 0xf0) = *(int *)(param_1 + 0xf0) + 1;
  }
  else if (*(int *)(param_1 + 0xf0) != 1) {
    return 0;
  }
  iVar1 = FUN_108573b4(*(undefined4 *)(param_1 + 400));
  if (iVar1 != 0) {
    iVar1 = *(int *)(param_1 + 400);
    if (*(char *)(iVar1 + 0xa0) == '\0') {
      *(undefined1 *)(iVar1 + 0xa0) = 1;
      FUN_10857178(iVar1 + 0x5c,iVar1 + 0xa1);
      *(undefined1 *)(iVar1 + 8) = 1;
    }
    *(undefined4 *)(param_1 + 0xf0) = 0;
    return 1;
  }
  return 0;
}

