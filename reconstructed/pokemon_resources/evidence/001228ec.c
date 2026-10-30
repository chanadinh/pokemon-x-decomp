/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_001228ec(undefined4 param_1,int *param_2)

{
  undefined4 uVar1;
  
  uVar1 = (**(code **)(*param_2 + 0x1c))(param_2);
  FUN_0012b0c8(param_2,param_1,uVar1,1);
  return;
}

