/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086fd18(uint *param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = param_2 & 0xff | (param_2 & 0xff) << 8;
  *param_1 = uVar1 | uVar1 << 0x10;
  *(short *)(param_1 + 1) = (short)uVar1;
  return;
}

