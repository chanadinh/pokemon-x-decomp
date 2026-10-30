/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086b520(uint param_1,undefined4 param_2)

{
  uint uVar1;
  
  uVar1 = DAT_10b07350;
  if (param_1 < 2) {
    FUN_10800394(0,0);
    uVar1 = DAT_10b07350;
  }
  do {
    if (0x5f < uVar1) {
LAB_1086b578:
      FUN_10800394(0,0);
      uVar1 = 0;
LAB_1086b58c:
      (&DAT_10b07354)[uVar1] = (short)param_1;
      (&DAT_10b07414)[uVar1] = param_2;
      (&DAT_10b07714)[uVar1] = 0;
      (&DAT_10b07594)[uVar1] = 0;
      (&DAT_10b07894)[uVar1] = 0;
      return;
    }
    if ((ushort)(&DAT_10b07354)[uVar1] == 0) {
LAB_1086b570:
      if (uVar1 < 0x60) goto LAB_1086b58c;
      goto LAB_1086b578;
    }
    if ((ushort)(&DAT_10b07354)[uVar1] == param_1) {
      FUN_10800394(0,0);
      goto LAB_1086b570;
    }
    uVar1 = uVar1 + 1;
  } while( true );
}

