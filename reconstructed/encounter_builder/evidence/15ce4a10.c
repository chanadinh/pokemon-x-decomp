/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_15ce4a10(int param_1,undefined4 param_2,undefined2 *param_3,undefined4 param_4)

{
  char cVar1;
  byte bVar2;
  undefined2 uVar3;
  undefined4 uVar4;
  byte *pbVar5;
  byte bVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  undefined4 uVar11;
  undefined4 local_78;
  undefined4 uStack_74;
  undefined4 local_70;
  undefined4 uStack_6c;
  undefined4 local_68;
  undefined4 uStack_64;
  undefined2 local_60;
  undefined1 local_5e;
  ushort local_5c;
  undefined2 local_5a;
  ushort local_58;
  char local_56;
  char local_55;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined1 local_44;
  uint local_40;
  uint local_3c;
  int iStack_34;
  undefined4 local_30;
  undefined2 *puStack_2c;
  undefined4 local_28;
  
  local_78 = 0xffffffff;
  uStack_74 = 0xffffffff;
  local_70 = 0xffffffff;
  uStack_6c = 3;
  local_58 = 0xffff;
  local_56 = -1;
  iVar8 = 0;
  local_5a = 0xff;
  local_55 = '\x01';
  uVar7 = 0;
  local_54 = 0;
  local_50 = 0;
  local_48 = 0xffff;
  local_4c = 0;
  local_44 = 0;
  iStack_34 = param_1;
  local_30 = param_2;
  puStack_2c = param_3;
  local_28 = param_4;
  do {
    iVar9 = iVar8 + 1;
    uVar7 = uVar7 + 2;
    *(undefined2 *)((int)&local_54 + iVar8 * 2) = 0xffff;
    iVar8 = iVar8 + 2;
    *(undefined2 *)((int)&local_54 + iVar9 * 2) = 0xffff;
  } while (uVar7 < 6);
  local_60 = *param_3;
  local_5c = (ushort)*(byte *)(param_3 + 2);
  local_5e = *(undefined1 *)((int)param_3 + 5);
  local_68 = *(undefined4 *)(param_1 + 0x9c);
  uStack_64 = 0;
  _static_::anon_segment_0_67e2c(local_60);
  if (*(char *)(param_3 + 8) == '\x01') {
    local_5a = 0;
  }
  else if (*(char *)(param_3 + 8) == '\x02') {
    local_5a = 1;
  }
  if (*(char *)(param_3 + 3) == '\x01') {
    uStack_6c = 2;
  }
  else {
    if (*(char *)(param_3 + 3) != '\x02') {
      bVar2 = (byte)(*(uint *)(param_1 + 0xa4) >> 3) & 2;
      local_55 = bVar2 + 1;
      if (*(char *)(param_1 + 0x85) == '\x02') {
        local_55 = bVar2 + 5;
      }
      if ((*(uint *)(param_1 + 0xa4) & 2) != 0) {
        local_55 = local_55 + (char)*(undefined2 *)(param_1 + 0xa2) * '\x02';
      }
      goto LAB_15ce4b60;
    }
    uStack_6c = 1;
  }
  local_70 = 0xffffffff;
LAB_15ce4b60:
  iVar8 = _static_::anon_segment_0_67e18(0x18);
  if ((iVar8 == 0xf) || (iVar8 = _static_::anon_segment_0_67e18(0x19), iVar8 == 0xf)) {
    local_44 = 3;
  }
  else if (*(char *)(param_1 + 0x85) == '\x02') {
    local_44 = 2;
  }
  else {
    local_44 = *(undefined1 *)((int)param_3 + 0x11);
  }
  cVar1 = *(char *)((int)param_3 + 7);
  if ((cVar1 == '\x01' || cVar1 == '\x02') || cVar1 == '\x03') {
    local_56 = cVar1 + -1;
  }
  if (*(char *)(param_1 + 0x85) == '\x02') {
    uVar11 = *(undefined4 *)(param_1 + 0x78);
    uVar4 = _static_::anon_segment_0_332a7c(uVar11);
    iVar8 = _static_::anon_segment_0_332f00(uVar11,uVar4);
    if (iVar8 == 0) {
      local_56 = -1;
    }
    else {
      local_56 = -2;
    }
  }
  if ((*(uint *)(param_1 + 0xa4) & 0x400) != 0) {
    local_58 = (ushort)*(byte *)(param_1 + 0x97);
  }
  iVar8 = _static_::anon_segment_0_22920(0x10,local_30);
  if (iVar8 == 0) {
    piVar10 = (int *)0x0;
  }
  else {
    piVar10 = (int *)_static_::anon_segment_0_29a5a8(iVar8,local_30,&local_78);
  }
  uVar7 = (uint)(ushort)param_3[1];
  if (uVar7 < 0x2cd) {
    if (uVar7 == 0) {
      iVar8 = 0;
      local_40 = 0;
      local_3c = 0;
      do {
        uVar3 = _static_::anon_segment_0_67e18(iVar8 + 0x11U & 0xff);
        iVar9 = iVar8 * 2;
        iVar8 = iVar8 + 1;
        *(undefined2 *)((int)&local_40 + iVar9) = uVar3;
      } while (iVar8 < 3);
      uVar7 = local_40 & 0xffff;
      if (uVar7 != local_40 >> 0x10) {
        bVar2 = _static_::anon_segment_0_35a908(100);
        pbVar5 = (byte *)(((*(int *)(param_1 + 0xa4) << 0x16) >> 0x1f) * -3 + DAT_15ce4d2c);
        if (bVar2 < *pbVar5) {
          uVar7 = local_40 & 0xffff;
        }
        else {
          bVar6 = *pbVar5 + pbVar5[1];
          if (bVar2 < bVar6) {
            uVar7 = local_40 >> 0x10;
          }
          else if (bVar2 < (byte)(pbVar5[2] + bVar6)) {
            uVar7 = local_3c & 0xffff;
          }
          else {
            uVar7 = 0;
          }
        }
      }
    }
    _static_::anon_segment_0_29d804(piVar10,uVar7);
  }
  _static_::anon_segment_0_29e814(local_28,piVar10);
  if (piVar10 != (int *)0x0) {
    (**(code **)(*piVar10 + 8))(piVar10);
  }
  return;
}

