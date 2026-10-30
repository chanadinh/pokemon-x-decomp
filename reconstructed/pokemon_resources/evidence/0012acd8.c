/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 * FUN_0012acd8(undefined4 *param_1,ushort param_2,int *param_3)

{
  undefined4 uVar1;
  undefined2 *puVar2;
  
  *param_1 = &DAT_00598570;
  *(undefined1 *)(param_1 + 3) = 0;
  *(undefined2 *)((int)param_1 + 10) = 0;
  *(ushort *)(param_1 + 2) = param_2;
  uVar1 = (**(code **)(*param_3 + 0x1c))(param_3);
  puVar2 = (undefined2 *)FUN_0012b0c8(param_3,(uint)param_2 << 1,uVar1,1);
  param_1[1] = puVar2;
  if (puVar2 != (undefined2 *)0x0) {
    *puVar2 = 0;
    *(undefined1 *)(param_1 + 3) = 1;
  }
  return param_1;
}

