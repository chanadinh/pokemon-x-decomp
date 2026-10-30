/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_0011e9dc(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  if (DAT_005e5288 == 0) {
    iVar1 = FUN_00122920(0x78,param_1);
    uVar2 = 0;
    if (iVar1 != 0) {
      uVar2 = FUN_00128288(iVar1,param_1,param_2,1);
    }
    DAT_005e5288 = uVar2;
    FUN_00127efc(uVar2,param_1);
    return;
  }
  return;
}

