/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5848(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  byte abStack_28 [8];
  undefined1 auStack_20 [5];
  byte local_1b;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 != param_3) || (*param_4 < 2)) {
    return;
  }
  uVar2 = FUN_108544a0(param_2,param_3);
  uVar3 = 0;
  do {
    iVar1 = FUN_108c86d4(uVar2,uVar3 & 0xff);
    if (iVar1 != 0) {
      FUN_1084bac4(uVar2,uVar3 & 0xff,0,auStack_20);
      uVar3 = (uint)local_1b;
      iVar1 = FUN_1084cb78(param_2,uVar3);
      if (iVar1 != 10) goto LAB_108b5918;
      uVar3 = FUN_10873158(param_2,param_3);
      iVar1 = FUN_1084bc20(param_2,uVar3 | 0x100,abStack_28);
      if (iVar1 != 0) {
        uVar3 = FUN_1084c7bc();
        uVar3 = (uint)abStack_28[uVar3 & 0xff];
        goto LAB_108b5918;
      }
      break;
    }
    uVar3 = uVar3 + 1;
  } while (uVar3 < 3);
  uVar3 = 0x1f;
LAB_108b5918:
  param_4[2] = uVar3;
  FUN_108658d4(4,uVar3);
  return;
}

