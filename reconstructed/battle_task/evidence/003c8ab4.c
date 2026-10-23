/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_003c8ab4(undefined1 *param_1,int param_2,int param_3,int param_4,undefined1 param_5)

{
  undefined2 uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined1 uVar6;
  undefined4 uVar7;
  
  uVar7 = DAT_003c8ce0;
  uVar6 = 0;
  uVar5 = *(undefined4 *)(*(int *)(param_2 + 0x1c) + 0x28);
  *param_1 = 9;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[4] = 1;
  param_1[5] = 1;
  param_1[6] = 1;
  param_1[7] = 1;
  param_1[8] = 0;
  param_1[9] = 0;
  *(undefined2 *)(param_1 + 10) = 0;
  *(undefined4 *)(param_1 + 0x10) = 0xffff;
  param_1[0x14] = 0;
  param_1[0x15] = 0;
  param_1[3] = 0;
  *(undefined2 *)(param_1 + 0x16) = 0;
  *(undefined4 *)(param_1 + 0x1c) = uVar7;
  param_1[0x19] = 0;
  *(uint *)(param_1 + 0x10) = (uint)*(ushort *)(*(int *)(*(int *)(param_2 + 0x1c) + 0x2c) + 0x10);
  uVar1 = _static_::anon_segment_0_3a33ac(uVar5);
  *(undefined2 *)(param_1 + 0x16) = uVar1;
  switch(*(undefined1 *)(*(int *)(param_2 + 0x2c) + 0x34)) {
  case 0xe:
  case 0xf:
  case 0x10:
    uVar6 = 2;
    break;
  case 0x11:
  case 0x12:
  case 0x13:
  case 0x14:
    uVar6 = 3;
  }
  param_1[2] = uVar6;
  param_1[0x18] = param_5;
  if (*(int *)(param_2 + 0x48) == 0) {
    if (param_3 == 0) {
      param_3 = FUN_004a3374(uVar5);
    }
    _static_::anon_segment_0_35a5c8(param_3,param_4,param_1 + 4);
    return;
  }
  iVar2 = DllField::anon_segment_0_e4be0();
  uVar3 = *(uint *)(*(int *)(iVar2 + 0xc) + 0x148);
  uVar4 = DllField::anon_segment_0_7de28(iVar2);
  switch(uVar4) {
  case 0:
    uVar7 = DAT_003c8cf0;
    break;
  case 2:
    uVar7 = DAT_003c8ce8;
    break;
  case 3:
    uVar7 = DAT_003c8cf8;
    break;
  case 4:
    uVar7 = DAT_003c8cec;
    break;
  case 5:
    uVar7 = DAT_003c8cf4;
    break;
  case 6:
    uVar7 = DAT_003c8ce4;
    break;
  case 7:
    uVar7 = DAT_003c8cfc;
  }
  *(undefined4 *)(param_1 + 0x1c) = uVar7;
  if (param_3 == 0) {
    param_3 = FUN_004a3374(uVar5);
  }
  if (param_4 == 0) {
    param_4 = FUN_003e0aa8(uVar3);
  }
  _static_::anon_segment_0_35a5c8(param_3,param_4,param_1 + 4);
  if (param_4 == 0) {
    switch(uVar3 >> 0x18) {
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x27:
      if (param_3 == 0x29 || param_3 == 0x3e) {
        param_1[6] = 0x29;
      }
      else {
        param_1[6] = 1;
      }
    }
  }
  return;
}

