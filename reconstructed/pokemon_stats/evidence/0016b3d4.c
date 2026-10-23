/* Unreviewed Ghidra pseudocode; types require assembly review. */

int FUN_0016b3d4(int param_1,int param_2)

{
  int iVar1;
  
  if (((*(uint **)(param_1 + 8))[1] & 4) == 0) {
    return (uint)*(byte *)(DAT_0016b450 + ((**(uint **)(param_1 + 8) & 0x3e000) >> 0xd) * 4 + 1) *
           0x38 + 8 + *(int *)(param_1 + 8);
  }
  iVar1 = DAT_0016b448;
  if (param_2 == 0) {
    iVar1 = DAT_0016b44c;
  }
  return iVar1;
}

