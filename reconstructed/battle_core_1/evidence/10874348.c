/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_10874348(int param_1)

{
  uint uVar1;
  
  uVar1 = 0;
  do {
    if ((1 << ((byte)(&DAT_108d10df)[uVar1] & 7) & 0xffU &
        (uint)*(byte *)(param_1 + 0x169 + (uint)((byte)(&DAT_108d10df)[uVar1] >> 3))) != 0) {
      return (&DAT_108d10df)[uVar1];
    }
    uVar1 = uVar1 + 1;
  } while (uVar1 < 4);
  return 0x12;
}

