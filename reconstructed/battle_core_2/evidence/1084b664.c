/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

char FUN_1084b664(uint param_1,char param_2)

{
  if ((param_1 & 1) == 0) {
    param_2 = param_2 << 1;
  }
  else {
    param_2 = param_2 * '\x02' + '\x01';
  }
  return param_2;
}

