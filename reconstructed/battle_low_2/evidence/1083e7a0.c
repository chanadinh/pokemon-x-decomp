/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_1083e7a0(int param_1)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x14ab) {
    DAT_108fcb84 = 0x14ab;
  }
  iVar1 = thunk_FUN_0012f7d0(*(undefined4 *)(*(int *)(param_1 + 400) + 0x4f4),0);
  return iVar1 != 0;
}

