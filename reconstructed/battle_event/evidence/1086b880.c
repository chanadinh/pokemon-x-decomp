/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086b880(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = DAT_10b07350;
  if (DAT_10b07350 == 0) {
    FUN_10800394(0,0);
    return;
  }
  for (; (uVar2 = uVar2 & 0xffff, uVar2 < 0x60 && ((&DAT_10b07354)[uVar2] != 0)); uVar2 = uVar2 + 1)
  {
    (&DAT_10b07354)[uVar2] = 0;
  }
  DAT_10b07350 = DAT_10b07350 - 1;
  (&DAT_10b07354)[DAT_10b07350] = 0;
  iVar1 = DAT_10b07350;
  do {
    DAT_10b07350 = iVar1;
    if (DAT_10b07350 == 0) {
      return;
    }
    iVar1 = DAT_10b07350 + -1;
  } while ((&DAT_10b07354)[DAT_10b07350 + -1] != 1);
  return;
}

