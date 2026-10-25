/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint _static_::anon_segment_0_679f4(short *param_1,short *param_2)

{
  short sVar1;
  short *psVar2;
  short *psVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  
  uVar4 = 0;
  do {
    sVar1 = *param_2;
    *param_1 = sVar1;
    psVar2 = param_2;
    uVar5 = uVar4;
    if (sVar1 == 0x10) {
      psVar2 = param_2 + (ushort)param_2[1] + 2;
      uVar6 = (int)psVar2 - (int)param_2;
      uVar5 = uVar4 + ((int)uVar6 >> 1);
      if (0 < (int)uVar6 >> 1) {
        psVar3 = param_1;
        if ((int)(uVar6 * 0x40000000) < 0) {
          param_2 = param_2 + 1;
          psVar3 = param_1 + 1;
          param_1[1] = *param_2;
        }
        for (iVar7 = (int)uVar6 >> 2; iVar7 != 0; iVar7 = iVar7 + -1) {
          psVar3[1] = param_2[1];
          param_2 = param_2 + 2;
          psVar3 = psVar3 + 2;
          *psVar3 = *param_2;
        }
        param_1 = (short *)((int)param_1 + (uVar6 & 0xfffffffe));
        goto LAB_00167a70;
      }
    }
    else {
LAB_00167a70:
      if (*param_1 == 0) {
        return uVar5;
      }
    }
    uVar4 = uVar5 + 1;
    param_1 = param_1 + 1;
    param_2 = psVar2 + 1;
    if (0x7ff < uVar4) {
      *param_1 = 0;
      return uVar5;
    }
  } while( true );
}

