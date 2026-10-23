/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0014f8cc(undefined4 param_1,int param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_0016b3d4(param_1,1);
  *(uint *)(iVar1 + 0x34) = *(uint *)(iVar1 + 0x34) & 0x7fffffff | param_2 << 0x1f;
  FUN_001685f0(param_1);
  return;
}

