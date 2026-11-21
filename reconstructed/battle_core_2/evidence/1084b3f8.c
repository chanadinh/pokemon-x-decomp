/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084b3f8(int param_1,byte param_2,undefined4 param_3)

{
  byte bVar1;
  byte *pbVar2;
  
  bVar1 = FUN_1086c2cc(param_1 + 0x120c,param_3);
  pbVar2 = *(byte **)(param_1 + 0xdc0);
  if (*pbVar2 < 10) {
    pbVar2[*pbVar2 + 1] = param_2;
    pbVar2[*pbVar2 + 0x15] = (byte)param_3;
    pbVar2[*pbVar2 + 0xb] = bVar1;
    *pbVar2 = *pbVar2 + 1;
  }
  return;
}

