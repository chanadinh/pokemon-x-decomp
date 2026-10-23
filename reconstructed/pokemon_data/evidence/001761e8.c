/* Unreviewed Ghidra pseudocode; types require assembly review. */

int FUN_001761e8(int param_1,int param_2)

{
  int iVar1;
  
  if (((*(uint **)(param_1 + 8))[1] & 4) == 0) {
    return (uint)*(byte *)(DAT_00176264 + ((**(uint **)(param_1 + 8) & 0x3e000) >> 0xd) * 4 + 3) *
           0x38 + 8 + *(int *)(param_1 + 8);
  }
  iVar1 = DAT_0017625c;
  if (param_2 == 0) {
    iVar1 = DAT_00176260;
  }
  return iVar1;
}

