/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

byte FUN_108c8714(int param_1,uint param_2)

{
  byte bVar1;
  byte bVar2;
  
  if (*(byte *)(param_1 + 0x152) <= param_2) {
    FUN_10800394(0,0);
  }
  bVar1 = *(byte *)(param_1 + param_2 * 0xe + 0x121);
  bVar2 = bVar1 & 0xf;
  if ((bVar1 & 0xf) != 0) {
    bVar2 = 1;
  }
  return bVar2;
}

