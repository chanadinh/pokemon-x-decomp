/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083f15c(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x14a3) {
    DAT_108fcb84 = 0x14a3;
  }
  FUN_108577b4(param_1[100]);
  iVar1 = param_1[100];
  FUN_1082d830();
  FUN_1082d6d8(iVar1 + 0x4dc,param_2,param_3);
  iVar1 = (**(code **)(*param_1 + 0xc0))(param_1);
  if (iVar1 == 0) {
    thunk_FUN_00425b1c(0x60000,0,0xffffffff,0);
    return;
  }
  return;
}

