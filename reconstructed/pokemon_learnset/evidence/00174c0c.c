/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_74c0c(int param_1,uint param_2,uint param_3)

{
  uint uVar1;
  undefined4 *puVar2;
  int iVar3;
  uint unaff_r5;
  undefined4 *puVar4;
  bool bVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  
  iVar3 = 0;
  if (param_2 != 0x2d2) {
    iVar3 = param_2 - 0x200;
  }
  if ((param_2 == 0x2d2 || iVar3 == 0xd3) || (DAT_00174ca8 < param_2)) {
    uVar1 = 0;
  }
  else {
    if (param_3 != 0) {
      iVar3 = *(int *)(DAT_00174ca4 + 4) + param_2 * 0x40;
      uVar1 = *(uint *)(iVar3 + 0x1c) & 0xffff;
      bVar5 = uVar1 == 0;
      if (!bVar5) {
        unaff_r5 = *(uint *)(iVar3 + 0x20) & 0xff;
      }
      if (!bVar5 && param_3 < unaff_r5) {
        uVar1 = uVar1 + (param_3 - 1);
      }
      if ((bVar5 || param_3 <= unaff_r5) && (!bVar5 && unaff_r5 != param_3)) goto LAB_00174c74;
    }
    uVar1 = param_2;
  }
LAB_00174c74:
  puVar2 = (undefined4 *)(*(int *)(DAT_00174ca4 + 4) + uVar1 * 0x40);
  puVar4 = *(undefined4 **)(param_1 + 8);
  uVar6 = puVar2[1];
  uVar7 = puVar2[2];
  uVar8 = puVar2[3];
  uVar9 = puVar2[4];
  uVar10 = puVar2[5];
  uVar11 = puVar2[6];
  uVar12 = puVar2[7];
  *puVar4 = *puVar2;
  puVar4[1] = uVar6;
  puVar4[2] = uVar7;
  puVar4[3] = uVar8;
  puVar4[4] = uVar9;
  puVar4[5] = uVar10;
  puVar4[6] = uVar11;
  puVar4[7] = uVar12;
  uVar6 = puVar2[9];
  uVar7 = puVar2[10];
  uVar8 = puVar2[0xb];
  uVar9 = puVar2[0xc];
  uVar10 = puVar2[0xd];
  uVar11 = puVar2[0xe];
  uVar12 = puVar2[0xf];
  puVar4[8] = puVar2[8];
  puVar4[9] = uVar6;
  puVar4[10] = uVar7;
  puVar4[0xb] = uVar8;
  puVar4[0xc] = uVar9;
  puVar4[0xd] = uVar10;
  puVar4[0xe] = uVar11;
  puVar4[0xf] = uVar12;
  *(short *)(param_1 + 4) = (short)param_2;
  *(char *)(param_1 + 6) = (char)param_3;
  return;
}

