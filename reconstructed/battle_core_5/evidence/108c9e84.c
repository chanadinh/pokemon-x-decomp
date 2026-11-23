/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c9e84(uint *param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = param_2 & 0xff | (param_2 & 0xff) << 8;
  uVar2 = uVar1 | uVar1 << 0x10;
  *param_1 = uVar2;
  param_1[1] = uVar2;
  *(short *)(param_1 + 2) = (short)uVar1;
  return;
}

