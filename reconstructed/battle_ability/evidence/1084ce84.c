/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084ce84(uint param_1,int param_2,undefined4 param_3,int param_4)

{
  longlong lVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar2 = DAT_10b07350;
  if (param_1 < 2) {
    FUN_10800394(0,0);
    uVar2 = DAT_10b07350;
  }
  for (; (uVar2 < 0x60 && ((ushort)(&DAT_10b07354)[uVar2] != 0)); uVar2 = uVar2 + 1) {
    if ((ushort)(&DAT_10b07354)[uVar2] == param_1) goto LAB_1084cef8;
  }
  FUN_10800394(0,0);
  uVar2 = 0xffffffff;
LAB_1084cef8:
  if (-1 < (int)uVar2) {
    if ((&DAT_10b07894)[uVar2] != '\x03') {
      FUN_10800394(0,0);
      return;
    }
    lVar1 = (longlong)(int)(&DAT_10b07414)[uVar2] * (longlong)param_2 + 0x800;
    uVar3 = (uint)lVar1 >> 0xc | (int)((ulonglong)lVar1 >> 0x20) * 0x100000;
    uVar4 = (&DAT_10b07594)[uVar2];
    if (uVar4 == 0) {
      param_4 = (&DAT_10b07714)[uVar2];
    }
    if (uVar4 != 0 || param_4 != 0) {
      if ((int)uVar3 < (int)(&DAT_10b07714)[uVar2]) {
        uVar3 = (&DAT_10b07714)[uVar2];
      }
      if ((int)uVar4 < (int)uVar3) {
        uVar3 = uVar4;
      }
    }
    (&DAT_10b07414)[uVar2] = uVar3;
  }
  return;
}

