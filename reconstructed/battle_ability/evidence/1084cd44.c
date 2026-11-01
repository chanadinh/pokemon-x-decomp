/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

/* WARNING: Removing unreachable block (ram,0x1084cec8) */

void FUN_1084cd44(undefined4 param_1,undefined4 param_2,uint param_3,int param_4)

{
  longlong lVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  
  iVar7 = param_4;
  uVar3 = FUN_1086b91c(3);
  if (uVar3 != param_3) {
    return;
  }
  uVar3 = FUN_10873158(param_2,param_3);
  if (uVar3 != 10) {
    uVar3 = FUN_1084bc20(param_2,uVar3 | 0x700,param_4);
    uVar8 = 0;
    if (uVar3 != 0) {
      do {
        if (*(byte *)(param_4 + uVar8) != param_3) {
          uVar4 = FUN_108544a0(param_2);
          iVar5 = FUN_10874420(uVar4,0x11);
          if (iVar5 == 0x39) goto LAB_1084ce54;
        }
        uVar8 = uVar8 + 1 & 0xff;
      } while (uVar8 < uVar3);
    }
  }
  uVar3 = FUN_10873158(param_2,param_3);
  if (uVar3 == 10) {
    return;
  }
  uVar3 = FUN_1084bc20(param_2,uVar3 | 0x700,param_4);
  uVar8 = 0;
  if (uVar3 == 0) {
    return;
  }
  do {
    if (*(byte *)(param_4 + uVar8) != param_3) {
      uVar4 = FUN_108544a0(param_2);
      iVar5 = FUN_10874420(uVar4,0x11);
      if (iVar5 == 0x3a) break;
    }
    uVar8 = uVar8 + 1 & 0xff;
    if (uVar3 <= uVar8) {
      return;
    }
  } while( true );
LAB_1084ce54:
  uVar2 = FUN_1086b91c(0x12);
  iVar5 = thunk_FUN_0039ddc0(uVar2);
  uVar3 = DAT_10b07350;
  if (iVar5 != 2) {
    return;
  }
  for (; (uVar3 < 0x60 && ((&DAT_10b07354)[uVar3] != 0)); uVar3 = uVar3 + 1) {
    if ((&DAT_10b07354)[uVar3] == 0x36) goto LAB_1084cef8;
  }
  FUN_10800394(0,0);
  uVar3 = 0xffffffff;
LAB_1084cef8:
  if (-1 < (int)uVar3) {
    if ((&DAT_10b07894)[uVar3] != '\x03') {
      FUN_10800394(0,0);
      return;
    }
    lVar1 = (longlong)(int)(&DAT_10b07414)[uVar3] * 0x1800 + 0x800;
    uVar8 = (uint)lVar1 >> 0xc | (int)((ulonglong)lVar1 >> 0x20) * 0x100000;
    uVar6 = (&DAT_10b07594)[uVar3];
    if (uVar6 == 0) {
      iVar7 = (&DAT_10b07714)[uVar3];
    }
    if (uVar6 != 0 || iVar7 != 0) {
      if ((int)uVar8 < (int)(&DAT_10b07714)[uVar3]) {
        uVar8 = (&DAT_10b07714)[uVar3];
      }
      if ((int)uVar6 < (int)uVar8) {
        uVar8 = uVar6;
      }
    }
    (&DAT_10b07414)[uVar3] = uVar8;
  }
  return;
}

