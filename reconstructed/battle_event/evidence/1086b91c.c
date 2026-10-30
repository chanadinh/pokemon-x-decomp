/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1086b91c(uint param_1)

{
  uint uVar1;
  
  uVar1 = DAT_10b07350;
  if (param_1 < 2) {
    FUN_10800394(0,0);
    uVar1 = DAT_10b07350;
  }
  for (; uVar1 < 0x60; uVar1 = uVar1 + 1) {
    if ((ushort)(&DAT_10b07354)[uVar1] == param_1) {
      return (&DAT_10b07414)[uVar1];
    }
    if ((ushort)(&DAT_10b07354)[uVar1] == 0) break;
  }
  FUN_10800394(0,0);
  return 0;
}

