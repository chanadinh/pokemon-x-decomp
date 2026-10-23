/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00174e6c(int param_1,uint param_2,int param_3)

{
  uint uVar1;
  ushort *puVar2;
  
  uVar1 = 0;
  if (param_2 >> 1 != 0) {
    do {
      puVar2 = (ushort *)(param_1 + uVar1 * 2);
      uVar1 = uVar1 + 1;
      param_3 = param_3 * DAT_00174eb0 + 0x6073;
      *puVar2 = *puVar2 ^ (ushort)((uint)param_3 >> 0x10);
    } while (uVar1 < param_2 >> 1);
    return;
  }
  return;
}

