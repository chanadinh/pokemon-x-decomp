/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_0011e884(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  if (DAT_005d1778 == 0) {
    iVar1 = FUN_00122920(0x78,param_1,param_3,param_4,param_4);
    iVar2 = 0;
    if (iVar1 != 0) {
      iVar2 = FUN_00128288(iVar1,param_1,param_2,1);
    }
    DAT_005d1778 = iVar2;
    iVar2 = FUN_00127bd8();
    iVar1 = FUN_0011c3cc(DAT_005d1778,iVar2 + -1);
    if (iVar2 * 0x40 + -0x40 != iVar1) {
      FUN_0011e864(0,0);
    }
    uVar3 = FUN_0012b094(param_1,iVar1);
    iVar1 = FUN_0011c3cc(DAT_005d1778,0);
    if (iVar1 != 0x40) {
      FUN_0011e864(0,0);
    }
    FUN_00127eb4(DAT_005d1778,iVar2 + -1,0,iVar2 * 0x40 + -0x40,uVar3);
    DAT_005d177c = uVar3;
  }
  return;
}

