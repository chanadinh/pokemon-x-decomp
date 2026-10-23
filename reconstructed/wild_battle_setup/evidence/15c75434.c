/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_15c75434(int param_1,int param_2,undefined1 param_3,undefined4 param_4,int param_5,
                 undefined4 param_6)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  
  piVar6 = *(int **)(param_2 + 0x1c);
  iVar5 = *piVar6;
  *(undefined2 *)(param_1 + 0x7c) = *(undefined2 *)(piVar6[10] + 0xc);
  *(undefined4 *)(param_1 + 0x80) = param_4;
  *(char *)(param_1 + 0x84) = (char)param_5;
  *(undefined1 *)(param_1 + 0x85) = param_3;
  uVar3 = _static_::anon_segment_0_3afe68(iVar5 + 0x129a8);
  *(undefined4 *)(param_1 + 0x9c) = uVar3;
  uVar1 = _static_::anon_segment_0_376b78(piVar6);
  *(undefined1 *)(param_1 + 0xa0) = uVar1;
  if (*(char *)(param_1 + 0x85) == '\x01') {
    *(uint *)(param_1 + 0xa4) = *(uint *)(param_1 + 0xa4) | 8;
  }
  if ((param_5 == 7 || param_5 == 8) || param_5 == 9) {
    *(uint *)(param_1 + 0xa4) = *(uint *)(param_1 + 0xa4) | 2;
  }
  uVar4 = _static_::anon_segment_0_40170(*piVar6 + 0x13148,0xa50);
  *(uint *)(param_1 + 0xa4) = uVar4 | *(uint *)(param_1 + 0xa4) & 0xfffffffe;
  iVar5 = _static_::anon_segment_0_335090(*piVar6 + 0x15fa8);
  if (iVar5 == 0) {
    uVar4 = *(uint *)(param_1 + 0xa4) | 4;
  }
  else {
    uVar4 = *(uint *)(param_1 + 0xa4) & 0xfffffffb;
  }
  *(uint *)(param_1 + 0xa4) = uVar4;
  iVar5 = _static_::anon_segment_0_3af77c(*piVar6 + 0x2d4,0x278,1,0);
  *(uint *)(param_1 + 0xa4) = *(uint *)(param_1 + 0xa4) & 0xffffffef | iVar5 << 4;
  uVar4 = _static_::anon_segment_0_3b0274(*piVar6 + 0xee8);
  *(uint *)(param_1 + 0xa4) = (uVar4 & 1) << 5 | *(uint *)(param_1 + 0xa4) & 0xffffffdf;
  uVar4 = _static_::anon_segment_0_2dfd30(*(undefined4 *)(*(int *)(param_2 + 0x1c) + 0x188));
  *(short *)(param_1 + 0xa2) = (short)uVar4;
  if (0x14 < uVar4) {
    *(undefined2 *)(param_1 + 0xa2) = 0x14;
  }
  *(int *)(param_1 + 0x78) = *piVar6 + 0x1ba78;
  iVar7 = piVar6[5];
  iVar5 = _static_::anon_segment_0_3a1e24(iVar7,1);
  if (iVar5 != 6) {
    uVar3 = _static_::anon_segment_0_29e508(iVar7,iVar5);
    uVar2 = _static_::anon_segment_0_52fd8();
    *(undefined2 *)(param_1 + 0x90) = uVar2;
    uVar2 = _static_::anon_segment_0_3a16c8(uVar3);
    *(undefined2 *)(param_1 + 0x92) = uVar2;
    uVar1 = _static_::anon_segment_0_3a0a24(uVar3);
    *(undefined1 *)(param_1 + 0x96) = uVar1;
    uVar1 = _static_::anon_segment_0_39feec(uVar3);
    *(undefined1 *)(param_1 + 0x95) = uVar1;
    uVar1 = _static_::anon_segment_0_3a0d40(uVar3);
    *(undefined1 *)(param_1 + 0x97) = uVar1;
    FUN_15ca2ac0(param_1,param_6);
    if ((*(uint *)(param_1 + 0xa4) & DAT_15c7560c) != 0) {
      uVar1 = _static_::anon_segment_0_68ae4(uVar3);
      *(undefined1 *)(param_1 + 0x98) = uVar1;
    }
  }
  return;
}

