/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 * FUN_0012b968(undefined4 *param_1)

{
  undefined4 *puVar1;
  
  *param_1 = &DAT_00598aac;
  *(undefined2 *)(param_1 + 1) = 0;
  param_1[2] = 0;
  puVar1 = (undefined4 *)FUN_00122920(0x24);
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1[6] = 0;
    puVar1[7] = 0;
    puVar1[8] = 0;
  }
  param_1[2] = puVar1;
  return param_1;
}

