/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1089e58c(uint param_1)

{
  uint uVar1;
  
  uVar1 = param_1;
  if (param_1 == 7) {
    uVar1 = 1;
  }
  if (param_1 < 8) {
    if ((param_1 != 7) && (uVar1 != 0)) {
      uVar1 = 3;
    }
  }
  else {
    uVar1 = 2;
  }
  return uVar1;
}

