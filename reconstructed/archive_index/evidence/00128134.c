/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00128134(undefined4 param_1,int *param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  
  uVar1 = (**(code **)(*param_2 + 0x1c))(param_2);
  puVar2 = (undefined4 *)FUN_0012b0c8(param_2,0x14,uVar1,1);
  if (puVar2 != (undefined4 *)0x0) {
    *puVar2 = &PTR_LAB_00597728;
    puVar2[1] = 0;
    puVar2[2] = 0;
    puVar2[3] = 0;
    puVar2[4] = 0;
  }
  return;
}

