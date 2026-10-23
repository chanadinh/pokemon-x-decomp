/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined8 FUN_00153014(undefined4 param_1,uint param_2)

{
  ushort uVar1;
  int iVar2;
  undefined4 extraout_r1;
  
  if (3 < param_2) {
    return 0;
  }
  FUN_00168660();
  iVar2 = FUN_0016b3d4(param_1,0);
  uVar1 = *(ushort *)(iVar2 + param_2 * 2 + 0x1a);
  FUN_001685f0(param_1);
  return CONCAT44(extraout_r1,(uint)uVar1);
}

