/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0014f324(int *param_1,int *param_2)

{
  uint uVar1;
  byte bVar2;
  undefined1 uVar3;
  undefined2 uVar4;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  
  iVar7 = param_2[1];
  iVar8 = param_2[2];
  iVar9 = param_2[3];
  iVar10 = param_2[4];
  iVar11 = param_2[5];
  iVar12 = param_2[6];
  iVar13 = param_2[7];
  *param_1 = *param_2;
  param_1[1] = iVar7;
  param_1[2] = iVar8;
  param_1[3] = iVar9;
  param_1[4] = iVar10;
  param_1[5] = iVar11;
  param_1[6] = iVar12;
  param_1[7] = iVar13;
  iVar10 = param_2[0xc];
  iVar7 = param_2[8];
  iVar8 = param_2[9];
  iVar9 = param_2[10];
  iVar11 = param_2[0xb];
  param_1[0xd] = param_2[0xd];
  param_1[0xc] = iVar10;
  param_1[8] = iVar7;
  param_1[9] = iVar8;
  param_1[10] = iVar9;
  param_1[0xb] = iVar11;
  if (param_1[1] == -1 && *param_1 == -1) {
    iVar7 = FUN_00167c0c();
    *param_1 = iVar7;
    param_1[1] = 0;
  }
  if (param_1[4] == -1 && param_1[5] == -1) {
    iVar7 = FUN_00167c0c();
    param_1[4] = iVar7;
    param_1[5] = 0;
  }
  iVar7 = param_1[2];
  iVar8 = param_1[3];
  if (iVar7 == -1 && iVar8 == 1) {
    uVar5 = FUN_00167c0c();
    iVar7 = FUN_001689a4(param_1[4],uVar5);
    param_1[2] = iVar7;
    param_1[3] = 0;
  }
  else if (iVar7 == -1 && iVar8 == 2) {
    uVar5 = FUN_00167c0c();
    iVar7 = FUN_00168944(param_1[4],uVar5);
    param_1[2] = iVar7;
    param_1[3] = 0;
  }
  else if ((iVar7 == -1 && iVar8 == 3) && (iVar7 = 0, *(char *)((int)param_1 + 0x23) != '\0')) {
    do {
      iVar8 = FUN_00167c0c();
      param_1[2] = iVar8;
      param_1[3] = 0;
      iVar8 = FUN_001688a4(param_1[4],iVar8);
      if (iVar8 != 0) break;
      iVar7 = iVar7 + 1;
    } while (iVar7 < (int)(uint)*(byte *)((int)param_1 + 0x23));
  }
  uVar1 = DAT_0014f6b4;
  if (100 < *(ushort *)(param_1 + 7)) {
    *(undefined2 *)(param_1 + 7) = 100;
  }
  if (*(byte *)(param_1 + 0xd) != 0) {
    if (5 < *(byte *)(param_1 + 0xd)) {
      *(undefined1 *)(param_1 + 0xd) = 0;
    }
    iVar8 = 0;
    iVar7 = 0;
    do {
      iVar9 = iVar8 + 1;
      iVar7 = iVar7 + 2;
      *(short *)((int)param_1 + iVar8 * 2 + 0x24) = (short)uVar1;
      iVar8 = iVar8 + 2;
      *(short *)((int)param_1 + iVar9 * 2 + 0x24) = (short)uVar1;
    } while (iVar7 < 6);
    bVar2 = 0;
    if ((char)param_1[0xd] != '\0') {
      do {
        do {
          uVar6 = FUN_00167bdc(6);
        } while (*(ushort *)((int)param_1 + (uVar6 & 0xff) * 2 + 0x24) != uVar1);
        *(undefined2 *)((int)param_1 + (uVar6 & 0xff) * 2 + 0x24) = 0x1f;
        bVar2 = bVar2 + 1;
      } while (bVar2 < *(byte *)(param_1 + 0xd));
    }
  }
  uVar6 = 0;
  do {
    if (*(ushort *)((int)param_1 + uVar6 * 2 + 0x24) == uVar1) {
      uVar4 = FUN_00167bdc(0x20);
      *(undefined2 *)((int)param_1 + uVar6 * 2 + 0x24) = uVar4;
    }
    uVar6 = uVar6 + 1 & 0xff;
  } while (uVar6 < 6);
  if (*(char *)((int)param_1 + 0x22) == -1) {
    uVar3 = FUN_00167bdc(2);
    *(undefined1 *)((int)param_1 + 0x22) = uVar3;
  }
  else if (*(char *)((int)param_1 + 0x22) == -2) {
    uVar3 = FUN_00167bdc(3);
    *(undefined1 *)((int)param_1 + 0x22) = uVar3;
  }
  if (*(ushort *)(param_1 + 8) == uVar1) {
    uVar4 = FUN_00167bdc(0x19);
    *(undefined2 *)(param_1 + 8) = uVar4;
  }
  if (*(char *)((int)param_1 + 0x1a) == -1) {
    FUN_00167ee4((short)param_1[6]);
    iVar7 = FUN_00167f44();
    if (iVar7 == 0) {
      _static_::anon_segment_0_67e2c((short)param_1[6],0);
      uVar3 = _static_::anon_segment_0_67e18(0x20);
      uVar3 = FUN_00167bdc(uVar3);
      *(undefined1 *)((int)param_1 + 0x1a) = uVar3;
    }
    else {
      *(undefined1 *)((int)param_1 + 0x1a) = 0;
    }
  }
  if (*(short *)((int)param_1 + 0x1e) != 0xff) goto LAB_0014f670;
  _static_::anon_segment_0_67e2c((short)param_1[6],*(undefined1 *)((int)param_1 + 0x1a));
  uVar6 = _static_::anon_segment_0_67e18(0x14);
  uVar4 = 0;
  if (uVar6 != 0) {
    if (uVar6 == 0xfe) {
LAB_0014f668:
      uVar4 = 1;
    }
    else if (uVar6 == 0xff) {
      uVar4 = 2;
    }
    else {
      iVar7 = FUN_00167bdc(0xfc);
      uVar4 = 0;
      if (iVar7 + 1U < uVar6) goto LAB_0014f668;
    }
  }
  *(undefined2 *)((int)param_1 + 0x1e) = uVar4;
LAB_0014f670:
  uVar4 = FUN_00168aa0((short)param_1[6],*(undefined1 *)((int)param_1 + 0x1a),
                       *(ushort *)((int)param_1 + 0x1e) & 0xff);
  *(undefined2 *)((int)param_1 + 0x1e) = uVar4;
  if (param_1[0xc] == uVar1) {
    _static_::anon_segment_0_67e2c((short)param_1[6],*(undefined1 *)((int)param_1 + 0x1a));
    iVar7 = _static_::anon_segment_0_67e18(0x16);
    param_1[0xc] = iVar7;
  }
  return;
}

