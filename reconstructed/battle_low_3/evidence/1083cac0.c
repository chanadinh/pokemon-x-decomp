/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1083cac0(int param_1,char *param_2)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x1479) {
    DAT_108fcb84 = 0x1479;
  }
  *param_2 = '\0';
  iVar1 = FUN_108a31a0(*(undefined4 *)(param_1 + 400));
  if (iVar1 != 0xb) {
    if (iVar1 < 0xc) {
      if (iVar1 == 2) {
        *param_2 = '\x04';
        return 1;
      }
      if (iVar1 != 10) {
        return 0;
      }
    }
    else if (iVar1 != 0xc && iVar1 != 0xd) {
      return 0;
    }
  }
  *param_2 = (char)iVar1 + -10;
  return 1;
}

