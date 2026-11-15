/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ba8b4(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 != param_3) {
    return;
  }
  uVar2 = FUN_108544a0(param_2,param_3);
  iVar1 = FUN_1084bb2c(uVar2,4);
  if ((iVar1 == 0) || (uVar3 = iVar1 - 1U & 0xff, 0x17 < uVar3)) {
    uVar3 = 0x1f;
  }
  else {
    iVar1 = FUN_1084cb78(param_2,uVar3);
    if (iVar1 != 10) goto LAB_108ba928;
  }
  FUN_108658d4(0x42,1);
LAB_108ba928:
  FUN_108683e4(param_2,param_3,0);
  *param_4 = 1;
  param_4[1] = uVar3;
  return;
}

