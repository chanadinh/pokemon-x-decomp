/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1084b648(uint *param_1)

{
  uint uVar1;
  
  if ((*param_1 & 0xf) == 1) {
    uVar1 = (*param_1 & 0xffffff) >> 8;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

