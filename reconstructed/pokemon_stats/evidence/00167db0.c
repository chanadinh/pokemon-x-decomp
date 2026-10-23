/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_67db0(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined1 extraout_r1;
  int iVar4;
  
  iVar3 = DAT_00167e14;
  iVar4 = *(int *)(DAT_00167e14 + 0x1c);
  anon_segment_0_74c0c(*(undefined4 *)(DAT_00167e14 + 0x18),param_1,param_2);
  uVar1 = anon_segment_0_75c78(*(undefined4 *)(iVar3 + 0x18),0x17);
  iVar2 = FUN_00175fec(*(undefined4 *)(iVar3 + 0x20),uVar1,iVar4);
  if (iVar2 != 0) {
    return;
  }
  FUN_00174dd8(iVar4,uVar1);
  iVar3 = *(int *)(iVar3 + 0x20);
  iVar2 = 0;
  if (*(char *)(iVar3 + 8) != '\0') {
    do {
      if (*(int *)(*(int *)(*(int *)(iVar3 + 4) + iVar2 * 4) + 4) == *(int *)(iVar4 + 4)) {
        return;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < (int)(uint)*(byte *)(iVar3 + 8));
  }
  FUN_0017c5b0(iVar4,*(undefined4 *)(*(int *)(iVar3 + 4) + (uint)*(byte *)(iVar3 + 9) * 4));
  anon_segment_0_5e958(*(byte *)(iVar3 + 9) + 1,*(undefined1 *)(iVar3 + 8));
  *(undefined1 *)(iVar3 + 9) = extraout_r1;
  return;
}

