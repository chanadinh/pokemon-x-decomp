/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086bb84(void)

{
  undefined2 *puVar1;
  
  for (; (DAT_10b07350 < 0x60 && ((&DAT_10b07354)[DAT_10b07350] != 0));
      DAT_10b07350 = DAT_10b07350 + 1) {
  }
  if (0x5e < DAT_10b07350) {
    thunk_FUN_0011e864(0,0);
    return;
  }
  puVar1 = &DAT_10b07354 + DAT_10b07350;
  DAT_10b07350 = DAT_10b07350 + 1;
  *puVar1 = 1;
  return;
}

