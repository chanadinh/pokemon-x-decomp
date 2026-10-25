/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_4f2b8(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined4 extraout_r1;
  uint uVar3;
  short *psVar4;
  
  iVar2 = DAT_0014f30c;
  psVar4 = *(short **)(DAT_0014f30c + 0x24);
  iVar1 = FUN_0016b25c(*(undefined4 *)(DAT_0014f30c + 0x28),param_1,param_2,psVar4);
  if (iVar1 != 0) {
    return;
  }
  FUN_00167ca4(psVar4,param_1,param_2);
  iVar2 = *(int *)(iVar2 + 0x28);
  uVar3 = 0;
  if (*(int *)(iVar2 + 8) != 0) {
    do {
      if ((**(short **)(*(int *)(iVar2 + 4) + uVar3 * 4) == *psVar4) &&
         (*(char *)(*(int *)(*(int *)(iVar2 + 4) + uVar3 * 4) + 2) == (char)psVar4[1])) {
        return;
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < *(uint *)(iVar2 + 8));
  }
  FUN_00175fb4(psVar4,*(undefined4 *)(*(int *)(iVar2 + 4) + *(int *)(iVar2 + 0xc) * 4));
  anon_segment_0_5e958(*(int *)(iVar2 + 0xc) + 1,*(undefined4 *)(iVar2 + 8));
  *(undefined4 *)(iVar2 + 0xc) = extraout_r1;
  return;
}

