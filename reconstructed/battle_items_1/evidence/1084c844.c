/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1084c844(int param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = 0;
  do {
    if ((5 < uVar1) || (uVar2 = (uint)(ushort)(&DAT_108d146a)[uVar1], uVar2 == 0)) {
      return 0;
    }
    if (0x25 < uVar2) {
      FUN_10800394(0,0);
    }
    uVar1 = uVar1 + 1;
  } while ((*(uint *)(param_1 + uVar2 * 4 + 0x1c) & 7) == 0);
  return uVar2;
}

