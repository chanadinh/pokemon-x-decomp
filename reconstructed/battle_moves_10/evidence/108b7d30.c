/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7d30(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    uVar3 = FUN_1084bb2c(uVar2,3);
    if (uVar3 != 0) {
      if (6 < uVar3) {
        uVar3 = 6;
      }
      iVar1 = FUN_1084c7bc(*(undefined2 *)(&DAT_108d4b38 + uVar3 * 2));
      if (iVar1 != 0) goto LAB_108b7da0;
    }
    iVar1 = FUN_1084b8e0(param_2,param_3);
    if (iVar1 != 0) {
LAB_108b7da0:
      FUN_108658d4(0x23,0x1b);
      return;
    }
  }
  return;
}

