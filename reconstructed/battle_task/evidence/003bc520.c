/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_003bc520(int param_1,int param_2,int param_3,uint param_4)

{
  undefined1 uVar1;
  char cVar2;
  short sVar3;
  ushort uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 uVar7;
  uint uVar8;
  undefined4 uVar9;
  int iVar10;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 uStack_28;
  
  uVar5 = _static_::anon_segment_0_2b284(8);
  uVar1 = *(undefined1 *)(param_3 + 0x8c);
  iVar10 = *(int *)(param_2 + 0x1c);
  local_30 = 0;
  FUN_003c8ab4(param_1 + 0x230,param_2,uVar1,*(undefined1 *)(param_3 + 0x8d));
  uVar9 = 0;
  iVar6 = _static_::anon_segment_0_22920(0x1c,uVar5);
  uVar7 = 0;
  if (iVar6 != 0) {
    uVar7 = _static_::anon_segment_0_1294c(iVar6,uVar5);
  }
  *(undefined4 *)(param_1 + 600) = uVar7;
  DllField::anon_segment_0_e47d8(param_3,uVar5,uVar7);
  if (1 < *(byte *)(*(int *)(param_1 + 600) + 0x18)) {
    uVar9 = 4;
  }
  local_2c = DllField::anon_segment_0_e4844(param_3);
  local_30 = uVar9;
  uStack_28 = uVar5;
  FUN_001cca9c(param_1 + 0x2c,param_2,*(undefined4 *)(param_1 + 600),param_1 + 0x230);
  *(int *)(param_1 + 0x20) = param_1 + 0x2c;
  sVar3 = *(short *)(*(int *)(iVar10 + 0x28) + 0xc);
  if (sVar3 == 0x103 || sVar3 == 0x11e) {
    FUN_001ceeb0();
  }
  cVar2 = *(char *)(param_3 + 0x8f);
  if (cVar2 == '\x01') {
    iVar6 = *(int *)(param_1 + 0x20);
    uVar8 = *(uint *)(iVar6 + 0xa8) | 0x80;
  }
  else {
    if (cVar2 != '\x02') {
      if (cVar2 == '\x03') {
        *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) =
             *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) | 0x200;
      }
      goto LAB_003bc668;
    }
    iVar6 = *(int *)(param_1 + 0x20);
    uVar8 = *(uint *)(iVar6 + 0xa8) | 4;
  }
  *(uint *)(iVar6 + 0xa8) = uVar8;
LAB_003bc668:
  if ((*(uint *)(param_3 + 0xa4) & 2) != 0) {
    *(undefined1 *)(param_1 + 0x2a) = 1;
    *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) = *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) | 1;
    iVar6 = *(int *)(param_1 + 0x20);
    uVar7 = FUN_003e0aa8(*(undefined4 *)(param_3 + 0x80));
    _static_::anon_segment_0_35a5c8(uVar1,uVar7,&local_30);
    *(undefined1 *)(iVar6 + 10) = local_30._2_1_;
  }
  uVar4 = *(ushort *)(*(int *)(param_2 + 0x1c) + 0x128) & 0xff;
  if (uVar4 == 3 || uVar4 == 0xb) {
    *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) = *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) | 0x4000
    ;
  }
  if ((uVar4 == 4 || uVar4 == 5) || (uVar4 == 6 || uVar4 == 7)) {
    *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) = *(uint *)(*(int *)(param_1 + 0x20) + 0xa8) | 0x8000
    ;
  }
  *(uint *)(param_1 + 0x1c) = param_4;
  *(undefined1 *)(param_1 + 0x28) = 1;
  if ((param_4 & 1) != 0) {
    *(undefined1 *)(param_1 + 0x29) = 1;
  }
  return;
}

