/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1084dab0(uint param_1,uint param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  
  if (8 < (int)(param_3 + param_4)) {
    FUN_10800394(0,0);
  }
  if (param_3 == 0) {
    FUN_10800394(0,0);
  }
  if (param_4 == 0) {
    FUN_10800394(0,0);
  }
  uVar1 = (1 << (param_3 & 0xff)) - 1U & 0xff;
  uVar2 = (1 << (param_4 & 0xff)) - 1U & 0xff;
  if ((int)uVar1 < (int)param_1) {
    FUN_10800394(0,0);
  }
  if ((int)uVar2 < (int)param_2) {
    FUN_10800394(0,0);
  }
  return param_2 & uVar2 | (param_1 & uVar1) << (param_4 & 0xff) & 0xff;
}

