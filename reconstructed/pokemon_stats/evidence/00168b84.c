/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint FUN_00168b84(int param_1,int param_2,int param_3,int param_4,uint param_5)

{
  uint uVar1;
  
  if (param_1 == 0x124) {
    uVar1 = 1;
  }
  else {
    uVar1 = (int)((ulonglong)((param_4 + param_3 * 2 + (param_5 >> 2)) * param_2) *
                  (ulonglong)DAT_00168bb8 >> 0x20) + param_2 + 10U & 0xffff;
  }
  return uVar1;
}

