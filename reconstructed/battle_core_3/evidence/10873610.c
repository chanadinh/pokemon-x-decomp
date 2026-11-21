/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10873610(uint *param_1,int param_2,uint param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = (param_3 & 0xf) << 4 | 1;
  uVar1 = uVar2 | param_2 << 8;
  *param_1 = uVar1;
  if (param_4 == 0) {
    uVar1 = uVar2 | param_2 << 8 & 0xefffffffU;
  }
  else {
    uVar1 = uVar1 | 0x10000000;
  }
  *param_1 = uVar1;
  return;
}

