/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10860c80(int param_1,int param_2)

{
  char cVar1;
  
  if (param_1 == 0x12 || param_2 == 0x12) {
    return 7;
  }
  cVar1 = (&DAT_108d12a8)[param_2 + param_1 * 0x12];
  if (cVar1 == '\0') {
    if (DAT_108fca01 == '\0') {
      return 0;
    }
  }
  else if (cVar1 == '\x02') {
    if (DAT_108fca01 == '\0') {
      return 6;
    }
  }
  else {
    if (cVar1 == '\x04') {
      return 7;
    }
    if (cVar1 != '\b') {
      FUN_10800394(0,0);
      return 0;
    }
    if (DAT_108fca01 != '\0') {
      return 6;
    }
  }
  return 8;
}

