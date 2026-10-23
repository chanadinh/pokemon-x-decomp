/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined8 FUN_00152efc(undefined4 param_1,uint param_2)

{
  byte bVar1;
  int iVar2;
  undefined4 extraout_r1;
  
  if (3 < param_2) {
    return 0;
  }
  FUN_00168660();
  iVar2 = FUN_0016b3d4(param_1,0);
  bVar1 = *(byte *)(iVar2 + param_2 + 0x22);
  FUN_001685f0(param_1);
  return CONCAT44(extraout_r1,(uint)bVar1);
}

