/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 * FUN_0038fc20(undefined4 *param_1)

{
  *param_1 = &DAT_005985b4;
  if (param_1[2] != 0) {
    if (param_1[7] != 0) {
      FUN_0013a50c();
    }
    if (param_1[6] != 0) {
      FUN_0013a50c();
    }
    if ((int *)param_1[2] != (int *)0x0) {
      (**(code **)(*(int *)param_1[2] + 4))();
    }
  }
  return param_1;
}

