/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_0014e984(short *param_1)

{
  short sVar1;
  short *psVar2;
  uint uVar3;
  
  sVar1 = *param_1;
  uVar3 = 0;
  while( true ) {
    if (sVar1 == 0) {
      return uVar3;
    }
    if (*param_1 == 0x10) {
      psVar2 = param_1 + (ushort)param_1[1] + 2;
      uVar3 = uVar3 + ((int)psVar2 - (int)param_1 >> 1);
    }
    else {
      psVar2 = param_1 + 1;
      uVar3 = uVar3 + 1;
    }
    if (0x7ff < uVar3) break;
    sVar1 = *psVar2;
    param_1 = psVar2;
  }
  return uVar3;
}

