/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 * FUN_0012b554(undefined4 *param_1)

{
  undefined4 *puVar1;
  
  *param_1 = &DAT_005989c0;
  *(undefined2 *)(param_1 + 1) = 0;
  *(undefined1 *)((int)param_1 + 6) = 0;
  param_1[2] = 0;
  puVar1 = (undefined4 *)FUN_00122920(0x40);
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
    puVar1[9] = 0;
    puVar1[10] = 0;
    puVar1[0xb] = 0;
    puVar1[0xc] = 0;
    puVar1[0xd] = 0;
    puVar1[0xe] = 0;
    puVar1[0xf] = 0;
  }
  param_1[2] = puVar1;
  return param_1;
}

