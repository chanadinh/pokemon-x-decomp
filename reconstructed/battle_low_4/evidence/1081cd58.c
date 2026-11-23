/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1081cd58(int param_1,int param_2,int param_3,int param_4)

{
  undefined4 uVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  
  uVar5 = 0xffff0000;
  uVar2 = (uint)*(byte *)(param_2 + 0x19);
  if (param_3 == 1) {
    if (param_4 == 5) {
      FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0xf9,uVar2,0xffff0000);
      goto LAB_1081cec4;
    }
    if (param_4 < 6) {
      if (param_4 == 1) {
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x11a,uVar2,0xffff0000);
        goto LAB_1081cec4;
      }
      if (param_4 == 2) {
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x13b,uVar2,0xffff0000);
        goto LAB_1081cec4;
      }
      if (param_4 == 3) {
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x129,uVar2,0xffff0000);
        goto LAB_1081cec4;
      }
      if (param_4 == 4) {
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x10b,uVar2,0xffff0000);
        goto LAB_1081cec4;
      }
    }
    else {
      if (param_4 == 6) {
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x162,uVar2,0xffff0000);
        goto LAB_1081cec4;
      }
      if (param_4 == 0xf) {
        uVar1 = *(undefined4 *)(param_1 + 0xc);
        uVar4 = 0x18;
        goto LAB_1081cee4;
      }
      if (param_4 == 0x14) {
        return;
      }
    }
LAB_1081cdc8:
    uVar1 = *(undefined4 *)(param_1 + 0xc);
    uVar4 = 0x4a;
    uVar3 = 0x69;
  }
  else {
    if (param_3 != 2) goto LAB_1081cdc8;
    uVar1 = *(undefined4 *)(param_1 + 0xc);
    uVar4 = 0xd2;
LAB_1081cee4:
    param_4 = -0x10000;
    uVar3 = 0x6a;
    uVar5 = uVar2;
  }
  FUN_1086b5e0(uVar1,uVar3,uVar4,uVar5,param_4);
LAB_1081cec4:
  *(byte *)(param_1 + 0x1093) = *(byte *)(param_1 + 0x1093) | 0x10;
  return;
}

