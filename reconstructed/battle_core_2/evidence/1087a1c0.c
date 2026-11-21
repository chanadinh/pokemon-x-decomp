/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1087a1c0(uint param_1)

{
  undefined4 uVar1;
  
  if (((param_1 & 7) == 1) && ((param_1 & 0x1f8) == 0x78)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

