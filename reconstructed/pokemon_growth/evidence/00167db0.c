/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00167db0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  undefined1 extraout_r1;
  int iVar4;
  
  iVar1 = DAT_005d17ac;
  FUN_00174c0c(DAT_005d17a8,param_1,param_2);
  uVar2 = FUN_00175c78(DAT_005d17a8,0x17);
  iVar3 = FUN_00175fec(DAT_005d17b0,uVar2,iVar1);
  if (iVar3 != 0) {
    return;
  }
  FUN_00174dd8(iVar1,uVar2);
  iVar3 = DAT_005d17b0;
  iVar4 = 0;
  if (*(char *)(DAT_005d17b0 + 8) != '\0') {
    do {
      if (*(int *)(*(int *)(*(int *)(DAT_005d17b0 + 4) + iVar4 * 4) + 4) == *(int *)(iVar1 + 4)) {
        return;
      }
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)(uint)*(byte *)(DAT_005d17b0 + 8));
  }
  FUN_0017c5b0(iVar1,*(undefined4 *)
                      (*(int *)(DAT_005d17b0 + 4) + (uint)*(byte *)(DAT_005d17b0 + 9) * 4));
  FUN_0015e958(*(byte *)(iVar3 + 9) + 1,*(undefined1 *)(iVar3 + 8));
  *(undefined1 *)(iVar3 + 9) = extraout_r1;
  return;
}

