/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint _static_::anon_segment_0_68ae4
               (int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined4 unaff_r4;
  uint uVar6;
  undefined4 unaff_r5;
  uint uVar7;
  undefined4 unaff_r6;
  undefined4 in_lr;
  
  iVar1 = FUN_0016b348(*(undefined4 *)(param_1 + 0xc));
  iVar2 = *(int *)(param_1 + 0xc);
  if (iVar1 != 0) {
    uVar7 = 0;
    if (*(int *)(iVar2 + 4) != 0) {
      FUN_00168660(iVar2);
      iVar1 = DAT_001762b0;
      if ((*(ushort *)(*(int *)(iVar2 + 8) + 4) & 4) == 0) {
        iVar1 = *(int *)(iVar2 + 4);
      }
      uVar7 = (uint)*(byte *)(iVar1 + 4);
      FUN_001685f0(iVar2);
    }
    return uVar7;
  }
  uVar3 = FUN_00152fe0();
  uVar4 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
  uVar7 = FUN_0016b45c(*(undefined4 *)(param_1 + 0xc));
  anon_segment_0_67db0(uVar3,uVar4,uVar7,param_4,unaff_r4,unaff_r5,unaff_r6,in_lr);
  uVar6 = 1;
  do {
    uVar5 = anon_segment_0_67f98(uVar6);
    if (uVar7 < uVar5) break;
    uVar6 = uVar6 + 1 & 0xff;
  } while (uVar6 < 0x65);
  if ((int)(uVar6 - 1) < 1) {
    uVar7 = 1;
  }
  else {
    uVar7 = uVar6 - 1 & 0xff;
  }
  return uVar7;
}

