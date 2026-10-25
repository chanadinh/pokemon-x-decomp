/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0013ff50(int param_1,ushort *param_2,uint param_3,int param_4,int param_5)

{
  ushort uVar1;
  undefined2 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  if (*(char *)(param_5 + 0xc) != '\0') {
    **(undefined2 **)(param_5 + 4) = 0;
    *(undefined2 *)(param_5 + 10) = 0;
  }
  if (*(int *)(*(int *)(param_1 + 0x18) + 8) == 0) {
    uVar3 = 0;
    uVar5 = (param_4 + 3) * DAT_001400d0 & 0xffff;
    if (param_3 != 0) {
      do {
        uVar1 = *param_2;
        if (*(char *)(param_5 + 0xc) == '\0') {
          return;
        }
        uVar4 = (uint)*(ushort *)(param_5 + 10);
        uVar6 = uVar4 + 1;
        if (uVar6 < *(ushort *)(param_5 + 8)) {
          *(short *)(param_5 + 10) = (short)uVar6;
          *(short *)(*(int *)(param_5 + 4) + uVar4 * 2) = (short)(uVar1 ^ uVar5);
          *(undefined2 *)(*(int *)(param_5 + 4) + (uint)*(ushort *)(param_5 + 10) * 2) = 0;
        }
        else {
          if (uVar6 != *(ushort *)(param_5 + 8) || (uVar1 ^ uVar5) != 0) goto LAB_00140084;
          *(short *)(param_5 + 10) = (short)uVar6;
          *(undefined2 *)(*(int *)(param_5 + 4) + uVar4 * 2) = 0;
        }
        uVar3 = uVar3 + 1;
        param_2 = param_2 + 1;
        uVar5 = uVar5 >> 0xd | (uVar5 & 0xffff1fff) << 3;
      } while (uVar3 < param_3);
    }
  }
  else {
    uVar5 = 0;
    if (param_3 != 0) {
      do {
        uVar1 = *param_2;
        if (*(char *)(param_5 + 0xc) == '\0') {
          return;
        }
        uVar4 = (uint)*(ushort *)(param_5 + 10);
        uVar3 = uVar4 + 1;
        if (uVar3 < *(ushort *)(param_5 + 8)) {
          *(short *)(param_5 + 10) = (short)uVar3;
          *(ushort *)(*(int *)(param_5 + 4) + uVar4 * 2) = uVar1;
          *(undefined2 *)(*(int *)(param_5 + 4) + (uint)*(ushort *)(param_5 + 10) * 2) = 0;
        }
        else {
          if (uVar3 != *(ushort *)(param_5 + 8) || uVar1 != 0) {
LAB_00140084:
            **(undefined2 **)(param_5 + 4) = 0;
            *(undefined2 *)(param_5 + 10) = 0;
            return;
          }
          *(short *)(param_5 + 10) = (short)uVar3;
          *(undefined2 *)(*(int *)(param_5 + 4) + uVar4 * 2) = 0;
        }
        uVar5 = uVar5 + 1;
        param_2 = param_2 + 1;
      } while (uVar5 < param_3);
    }
  }
  if (*(char *)(param_5 + 0xc) == '\0') {
    return;
  }
  uVar2 = _static_::anon_segment_0_4e984(*(undefined4 *)(param_5 + 4));
  *(undefined2 *)(param_5 + 10) = uVar2;
  return;
}

