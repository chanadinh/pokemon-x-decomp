/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1087bfa0(uint param_1,undefined4 *param_2)

{
  uint uVar1;
  
  uVar1 = DAT_10b07350;
  if (param_1 < 2) {
    FUN_10800394(0,0);
    uVar1 = DAT_10b07350;
  }
  while( true ) {
    if (0x5f < uVar1) {
      return 0;
    }
    if ((ushort)(&DAT_10b07354)[uVar1] == param_1) break;
    if ((ushort)(&DAT_10b07354)[uVar1] == 0) {
      return 0;
    }
    uVar1 = uVar1 + 1;
  }
  *param_2 = (&DAT_10b07414)[uVar1];
  return 1;
}

