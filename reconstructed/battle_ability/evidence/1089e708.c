/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1089e708(int param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = (uint)(param_1 * param_2) >> 0xc;
  if (0x800 < (param_1 * param_2 & 0xfffU)) {
    uVar1 = uVar1 + 1;
  }
  return uVar1;
}

