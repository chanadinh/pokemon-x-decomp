/* Unreviewed Ghidra pseudocode; types require assembly review. */

short FUN_00174e0c(short *param_1,uint param_2)

{
  short *psVar1;
  short sVar2;
  short *psVar3;
  short sVar4;
  
  sVar2 = 0;
  if (1 < param_2) {
    psVar3 = param_1 + -1;
    if ((int)(param_2 * 0x40000000) < 0) {
      sVar2 = *param_1;
      psVar3 = param_1;
    }
    sVar4 = 0;
    for (param_2 = param_2 >> 2; param_2 != 0; param_2 = param_2 - 1) {
      psVar1 = psVar3 + 1;
      psVar3 = psVar3 + 2;
      sVar2 = sVar2 + *psVar1;
      sVar4 = sVar4 + *psVar3;
    }
    sVar2 = sVar2 + sVar4;
  }
  return sVar2;
}

