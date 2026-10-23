/* Unreviewed Ghidra pseudocode; types require assembly review. */

byte FUN_00152fa8(undefined4 param_1)

{
  byte bVar1;
  int iVar2;
  
  FUN_00168660();
  iVar2 = FUN_0016b358(param_1,0);
  bVar1 = *(byte *)(iVar2 + 0x15);
  FUN_001685f0(param_1);
  return bVar1 >> 3;
}

