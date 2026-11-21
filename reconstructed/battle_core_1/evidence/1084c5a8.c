/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084c5a8(uint param_1,uint param_2)

{
  undefined4 uVar1;
  
  if ((param_1 == 0x1f || param_2 == 0x1f) || (0xb < param_1 != 0xb < param_2)) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

