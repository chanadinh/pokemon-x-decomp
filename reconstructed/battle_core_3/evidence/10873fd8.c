/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10873fd8(int param_1,undefined4 param_2)

{
  byte bVar1;
  
  bVar1 = *(byte *)(param_1 + 3);
  if (bVar1 >> 4 < 8) {
    *(byte *)(param_1 + 3) = bVar1 & 0xf | (bVar1 >> 4) * '\x10' + 0x10;
    *(undefined4 *)(param_1 + (uint)(bVar1 >> 4) * 4 + 4) = param_2;
  }
  return;
}

