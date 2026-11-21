/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10876cf4(uint *param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = *param_1;
  uVar1 = (param_2 & 0x3f) << 3;
  if (((uVar2 & 7) == 2) && ((uVar2 & 0x1ff) >> 3 < 8)) {
    uVar2 = uVar2 & 0xfffffe07 | uVar1;
    *param_1 = uVar2;
  }
  if (((uVar2 & 7) == 4) && ((uVar2 & 0x1ff) >> 3 < 8)) {
    *param_1 = uVar1 | uVar2 & 0xfffffe07;
  }
  return;
}

