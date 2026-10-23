/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0014fcb4(int param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  if (*(int *)(param_1 + 4) != 0) {
    FUN_00168660(param_1);
    puVar1 = DAT_0014fd00;
    if ((*(ushort *)(*(int *)(param_1 + 8) + 4) & 4) == 0) {
      puVar1 = *(undefined4 **)(param_1 + 4);
    }
    *puVar1 = param_2;
    FUN_001685f0(param_1);
    return;
  }
  return;
}

