/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1089e7f0(uint param_1,undefined4 param_2)

{
  switch(param_2) {
  case 0:
    param_1 = 0;
switchD_1089e7fc_caseD_7:
    return param_1;
  case 1:
    return param_1 >> 6;
  case 2:
    return param_1 >> 5;
  case 3:
    return param_1 >> 4;
  case 4:
    return param_1 >> 3;
  case 5:
    return param_1 >> 2;
  case 6:
    return param_1 >> 1;
  case 7:
    goto switchD_1089e7fc_caseD_7;
  case 8:
    return param_1 << 1;
  case 9:
    return param_1 << 2;
  case 10:
    return param_1 << 3;
  case 0xb:
    return param_1 << 4;
  case 0xc:
    return param_1 << 5;
  case 0xd:
    return param_1 << 6;
  default:
    FUN_10800394(0,0);
    return param_1;
  }
}

