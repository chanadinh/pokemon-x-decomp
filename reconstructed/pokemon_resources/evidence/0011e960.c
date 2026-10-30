/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_0011e960(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  
  if (DAT_005d17ec == 0) {
    iVar1 = FUN_00122920(0x78,param_1);
    iVar2 = 0;
    if (iVar1 != 0) {
      iVar2 = FUN_00128288(iVar1,param_1,param_2,1);
    }
    DAT_005d17ec = iVar2;
    FUN_00127efc(iVar2,param_1);
  }
  iVar2 = DAT_005d17e8;
  if (DAT_005d17e8 == 0) {
    iVar1 = FUN_00122920(0xc,param_1);
    iVar2 = 0;
    if (iVar1 != 0) {
      iVar2 = FUN_0012b554(iVar1,param_1);
    }
  }
  DAT_005d17e8 = iVar2;
  return;
}

