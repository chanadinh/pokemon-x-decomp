/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint FUN_0016b318(undefined4 param_1)

{
  int iVar1;
  uint uVar2;
  
  FUN_00168660();
  iVar1 = FUN_0016b3d4(param_1,0);
  uVar2 = *(uint *)(iVar1 + 0x34);
  FUN_001685f0(param_1);
  return uVar2 & 0x1f;
}

