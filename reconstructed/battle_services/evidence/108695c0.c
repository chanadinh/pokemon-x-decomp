/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108695c0(int param_1,int param_2,uint param_3,undefined4 *param_4)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  
  if (param_3 == 0) {
    return;
  }
  uVar1 = *(undefined1 *)(param_2 + 0x19);
  if (param_4 != (undefined4 *)0x0) {
    if (0x25 < param_3) {
      FUN_10800394(0,0);
    }
    *param_4 = *(undefined4 *)(param_2 + param_3 * 4 + 0x1c);
  }
  if (5 < param_3) {
    FUN_1086fd8c(param_2,param_3);
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x12,uVar1,param_3);
    return;
  }
  uVar2 = 1;
  do {
    *(undefined4 *)(param_2 + uVar2 * 4 + 0x1c) = 0;
    *(undefined1 *)(param_2 + uVar2 + 0xb4) = 0;
    if ((uVar2 & 0xffff) == 2) {
      *(undefined4 *)(param_2 + 0x40) = 0;
      *(undefined1 *)(param_2 + 0xbd) = 0;
    }
    uVar2 = uVar2 + 1;
  } while (uVar2 < 6);
  FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x11,uVar1);
  iVar3 = FUN_1086c2cc(param_1 + 0x120c,uVar1);
  if (iVar3 == 10) {
    return;
  }
  FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x3c,uVar1,0);
  return;
}

