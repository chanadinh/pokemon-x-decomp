/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined1 * _static_::anon_segment_0_35a5c8(int param_1,int param_2,undefined1 *param_3)

{
  int iVar1;
  short *psVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  short sVar6;
  undefined1 *puVar7;
  uint uVar8;
  
  if (param_2 == 0) {
    param_2 = param_1;
  }
  puVar7 = (undefined1 *)(DAT_0045a790 + param_1 * 2);
  iVar4 = DAT_0045a790 + 0x82;
  *param_3 = *puVar7;
  iVar3 = DAT_0045a794;
  iVar1 = param_2 * 5 + iVar4;
  param_3[1] = puVar7[1];
  param_3[2] = *(undefined1 *)(iVar4 + param_2 * 5);
  param_3[3] = *(undefined1 *)(iVar1 + 1);
  param_3[4] = *(undefined1 *)(iVar1 + 2);
  param_3[8] = *(undefined1 *)(iVar1 + 3);
  param_3[9] = *(undefined1 *)(iVar1 + 4);
  param_3[5] = 0;
  if (*(int *)(iVar3 + 0x14) == 0) {
    anon_segment_0_1e864(0,0);
  }
  iVar1 = anon_segment_0_74bb4(*(undefined4 *)(iVar3 + 0x14),100);
  iVar4 = 0;
  iVar5 = DAT_0045a798 + (uint)(byte)param_3[4] * 0xc;
  do {
    uVar8 = (uint)*(byte *)(iVar5 + iVar4 * 2 + 5);
    if (iVar1 < (int)uVar8) {
      param_3[5] = *(undefined1 *)(iVar5 + iVar4 * 2 + 4);
      break;
    }
    iVar4 = iVar4 + 1;
    iVar1 = iVar1 - uVar8;
  } while (iVar4 < 3);
  *(undefined2 *)(param_3 + 6) = 0;
  if ((byte)param_3[5] != 0) {
    iVar4 = DAT_0045a79c + (uint)(byte)param_3[5] * 0x18;
    sVar6 = *(short *)(iVar4 + 8);
    iVar5 = 3;
    uVar8 = (uint)(*(short *)(iVar4 + 6) != 0);
    iVar1 = 0;
    psVar2 = (short *)(iVar4 + 6);
    do {
      if (sVar6 != 0) {
        uVar8 = uVar8 + 1;
      }
      sVar6 = psVar2[3];
      if (psVar2[2] != 0) {
        iVar1 = iVar1 + 1;
      }
      iVar5 = iVar5 + -1;
      psVar2 = psVar2 + 2;
    } while (iVar5 != 0);
    iVar1 = uVar8 + iVar1;
    if (*(short *)(iVar4 + 0x14) != 0) {
      iVar1 = iVar1 + 1;
    }
    if (0 < iVar1) {
      if (*(int *)(iVar3 + 0x14) == 0) {
        anon_segment_0_1e864(0,0);
      }
      iVar3 = anon_segment_0_74bb4(*(undefined4 *)(iVar3 + 0x14),iVar1);
      iVar1 = 0;
      do {
        if (*(short *)(iVar4 + iVar1 * 2 + 6) != 0) {
          if (iVar3 == 0) {
            *(undefined2 *)(param_3 + 6) = *(undefined2 *)(iVar4 + iVar1 * 2 + 6);
            return param_3;
          }
          iVar3 = iVar3 + -1;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < 8);
    }
  }
  return param_3;
}

