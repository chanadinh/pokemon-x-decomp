/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108658d4(uint param_1,undefined4 param_2)

{
  byte bVar1;
  uint uVar2;
  
  uVar2 = DAT_10b07350;
  if (param_1 < 2) {
    FUN_10800394(0,0);
    uVar2 = DAT_10b07350;
  }
  for (; (uVar2 < 0x60 && ((ushort)(&DAT_10b07354)[uVar2] != 0)); uVar2 = uVar2 + 1) {
    if ((ushort)(&DAT_10b07354)[uVar2] == param_1) goto LAB_10865948;
  }
  FUN_10800394(0,0);
  uVar2 = 0xffffffff;
LAB_10865948:
  if (-1 < (int)uVar2) {
    bVar1 = (&DAT_10b07894)[uVar2];
    if (bVar1 < 2) {
      (&DAT_10b07354)[uVar2] = (short)param_1;
      (&DAT_10b07414)[uVar2] = param_2;
      if ((&DAT_10b07894)[uVar2] == '\x01') {
        (&DAT_10b07894)[uVar2] = 2;
      }
      return 1;
    }
    if (bVar1 == 4 || bVar1 == 3) {
      FUN_10800394(0,0);
    }
  }
  return 0;
}

