/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined2 FUN_0016b224(int param_1,uint param_2)

{
  if (*(byte *)(param_1 + 8) <= param_2) {
    param_2 = 0;
  }
  return *(undefined2 *)(*(int *)(param_1 + 4) + param_2 * 4);
}

