/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084db70(uint *param_1,undefined4 param_2)

{
  uint uVar1;
  
  if (0x176c < *param_1) {
    thunk_FUN_0011e864(0,0);
  }
  uVar1 = *param_1;
  *param_1 = uVar1 + 1;
  *(char *)((int)param_1 + uVar1 + 8) = (char)((uint)param_2 >> 0x18);
  uVar1 = *param_1;
  *param_1 = uVar1 + 1;
  *(char *)((int)param_1 + uVar1 + 8) = (char)((uint)param_2 >> 0x10);
  uVar1 = *param_1;
  *param_1 = uVar1 + 1;
  *(char *)((int)param_1 + uVar1 + 8) = (char)((uint)param_2 >> 8);
  uVar1 = *param_1;
  *param_1 = uVar1 + 1;
  *(char *)((int)param_1 + uVar1 + 8) = (char)param_2;
  return;
}

