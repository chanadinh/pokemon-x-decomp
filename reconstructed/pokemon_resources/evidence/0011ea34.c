/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 * FUN_0011ea34(undefined4 *param_1,undefined4 param_2,uint param_3)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  
  param_1[1] = 0;
  *param_1 = &DAT_005989e0;
  *(char *)(param_1 + 2) = (char)param_3;
  *(undefined1 *)((int)param_1 + 9) = 0;
  uVar1 = FUN_001228ec(param_3 << 2,param_2);
  uVar3 = 0;
  param_1[1] = uVar1;
  if (param_3 != 0) {
    do {
      iVar2 = FUN_00122920(0x19c,param_2);
      uVar1 = 0;
      if (iVar2 != 0) {
        uVar1 = FUN_0012b6a4();
      }
      *(undefined4 *)(param_1[1] + uVar3 * 4) = uVar1;
      uVar3 = uVar3 + 1;
    } while (uVar3 < param_3);
  }
  return param_1;
}

