/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_10871408(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = param_1 & 7;
  uVar1 = (param_1 & 0x1ffffff) >> 9;
  if (uVar2 != 1 && uVar2 != 3) {
    if (uVar2 == 4) {
      uVar1 = (param_1 & 0x7fffffff) >> 0xf;
    }
    else if (uVar2 != 2) {
      uVar1 = 0;
    }
  }
  return uVar1;
}

