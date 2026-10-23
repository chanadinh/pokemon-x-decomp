/* Unreviewed Ghidra pseudocode; types require assembly review. */

int FUN_00176168(int param_1,int param_2)

{
  int iVar1;
  
  if (((*(uint **)(param_1 + 8))[1] & 4) == 0) {
    return (uint)*(byte *)(DAT_001761e4 + ((**(uint **)(param_1 + 8) & 0x3e000) >> 0xd) * 4 + 2) *
           0x38 + 8 + *(int *)(param_1 + 8);
  }
  iVar1 = DAT_001761dc;
  if (param_2 == 0) {
    iVar1 = DAT_001761e0;
  }
  return iVar1;
}

