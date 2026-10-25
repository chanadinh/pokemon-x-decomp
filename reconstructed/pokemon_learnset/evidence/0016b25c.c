/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 FUN_0016b25c(int param_1,uint param_2,uint param_3,undefined4 param_4)

{
  uint uVar1;
  
  uVar1 = 0;
  if (*(int *)(param_1 + 8) != 0) {
    do {
      if ((**(ushort **)(*(int *)(param_1 + 4) + uVar1 * 4) == param_2) &&
         (*(byte *)(*(int *)(*(int *)(param_1 + 4) + uVar1 * 4) + 2) == param_3)) {
        FUN_00175fb4(*(undefined4 *)(*(int *)(param_1 + 4) + uVar1 * 4),param_4);
        return 1;
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < *(uint *)(param_1 + 8));
  }
  return 0;
}

