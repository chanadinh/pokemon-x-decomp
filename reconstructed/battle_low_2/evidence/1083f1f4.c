/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083f1f4(int *param_1)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x14d8) {
    DAT_108fcb84 = 0x14d8;
  }
  FUN_10860270(param_1[100] + 0x4ec,*(undefined4 *)(param_1[100] + 0x4e0));
  iVar1 = (**(code **)(*param_1 + 0xc0))(param_1);
  if (iVar1 == 0) {
    thunk_FUN_00425b1c(0x60000,0,0xffffffff,0);
    return;
  }
  return;
}

