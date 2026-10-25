/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00167ca4(ushort *param_1,uint param_2,uint param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  uint uVar3;
  char cVar4;
  int iVar5;
  bool bVar6;
  undefined8 uVar7;
  
  puVar1 = DAT_00167da4;
  uVar2 = (uint)*param_1;
  bVar6 = uVar2 == param_2;
  if (bVar6) {
    uVar2 = (uint)(byte)param_1[1];
  }
  if (bVar6 && uVar2 == param_3) {
    return;
  }
  iVar5 = 0;
  if (param_2 != 0x2d2) {
    iVar5 = param_2 - 0x200;
  }
  if ((param_2 == 0x2d2 || iVar5 == 0xd3) || (DAT_00167da8 < param_2)) {
    uVar2 = 0;
  }
  else {
    if (param_3 != 0) {
      _static_::anon_segment_0_74c0c(*DAT_00167da4,param_2,0);
      iVar5 = _static_::anon_segment_0_75c78(*puVar1,0x1e);
      uVar7 = _static_::anon_segment_0_75c78(*puVar1,0x20);
      uVar2 = (uint)((ulonglong)uVar7 >> 0x20);
      uVar3 = (uint)uVar7;
      bVar6 = iVar5 == 0;
      if (!bVar6 && param_3 < uVar3) {
        uVar2 = (iVar5 + param_3) - 1;
      }
      if ((bVar6 || param_3 <= uVar3) && (!bVar6 && uVar3 != param_3)) goto LAB_00167d40;
    }
    uVar2 = param_2;
  }
LAB_00167d40:
  _static_::anon_segment_0_38214(puVar1[1],uVar2,*(undefined4 *)(param_1 + 2));
  cVar4 = '\0';
  iVar5 = 0;
  do {
    if ((*(ushort *)(*(int *)(param_1 + 2) + iVar5 * 4) == DAT_00167dac) &&
       (*(ushort *)(*(int *)(param_1 + 2) + iVar5 * 4 + 2) == DAT_00167dac)) break;
    cVar4 = cVar4 + '\x01';
    iVar5 = iVar5 + 1;
  } while (iVar5 < 0x1a);
  *(char *)(param_1 + 4) = cVar4;
  *param_1 = (ushort)param_2;
  *(char *)(param_1 + 1) = (char)param_3;
  return;
}

