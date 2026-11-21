/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_10839818(int param_1)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0xfc4) {
    DAT_108fcb84 = 0xfc4;
  }
  iVar1 = FUN_10858cb8(param_1,0);
  if (iVar1 != 0) {
    FUN_1085db0c(param_1,0);
    FUN_10859cc4(*(int *)(param_1 + 400) + 0x280);
  }
  return iVar1 != 0;
}

