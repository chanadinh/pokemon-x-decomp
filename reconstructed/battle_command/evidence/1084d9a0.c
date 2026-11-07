/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1084d9a0(int param_1,uint param_2,uint param_3,uint param_4,uint param_5,uint param_6,
                 uint param_7)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  if (param_1 * 8 < (int)(param_7 + 10)) {
    FUN_10800394(0,0);
  }
  if (param_5 == 0) {
    FUN_10800394(0,0);
  }
  if (param_6 == 0) {
    FUN_10800394(0,0);
  }
  if (param_7 == 0) {
    FUN_10800394(0,0);
  }
  uVar1 = (1 << (param_5 & 0xff)) - 1U & 0xffff;
  uVar2 = (1 << (param_6 & 0xff)) - 1U & 0xffff;
  uVar3 = (1 << (param_7 & 0xff)) - 1U & 0xffff;
  if ((int)uVar1 < (int)param_2) {
    FUN_10800394(0,0);
  }
  if ((int)uVar2 < (int)param_3) {
    FUN_10800394(0,0);
  }
  if ((int)uVar3 < (int)param_4) {
    FUN_10800394(0,0);
  }
  return (param_2 & uVar1) << (param_6 + param_7 & 0xff) | (param_3 & uVar2) << (param_7 & 0xff) |
         param_4 & uVar3;
}

