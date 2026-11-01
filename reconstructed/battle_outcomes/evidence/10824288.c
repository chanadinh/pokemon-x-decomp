/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10824288(undefined4 *param_1,int param_2)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  bool bVar6;
  
  bVar1 = *(byte *)(param_2 + 0x4c);
  iVar4 = param_1[2];
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar4 = iVar4 + (uint)bVar1 * 4;
  if (*(int *)(iVar4 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  uVar5 = *(undefined4 *)(iVar4 + 0x84);
  iVar4 = FUN_10865c4c(param_1);
  if (iVar4 == 0) {
    uVar3 = (uint)*(byte *)(param_2 + 0x4c);
    iVar4 = param_1[2];
    if (0x17 < uVar3) {
      FUN_10800394(0,0);
    }
    iVar4 = iVar4 + uVar3 * 4;
    if (*(int *)(iVar4 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    cVar2 = *(char *)(*(int *)(iVar4 + 0x84) + 0x171);
    if ((cVar2 == '\0') || (0x17 < (byte)(cVar2 - 1U))) {
      iVar4 = param_1[2];
      if (0x17 < uVar3) {
        FUN_10800394(0,0);
      }
      iVar4 = iVar4 + uVar3 * 4;
      if (*(int *)(iVar4 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      uVar3 = *(uint *)(*(int *)(iVar4 + 0x84) + 0xa0);
      bVar6 = (uVar3 & 7) == 0;
      if (bVar6) {
        uVar3 = (uint)*(byte *)(param_1 + 6);
      }
      if (bVar6 && uVar3 == 0) {
        FUN_1086bbe8(param_1,param_2 + 4);
        iVar4 = FUN_10866c68(param_1,uVar5);
        if (iVar4 != 0) {
          uVar5 = FUN_10873164(param_1[1],param_1[2],*(undefined1 *)(param_2 + 0x4c));
          FUN_10810cc0(*param_1,uVar5);
          FUN_1086bbe8(param_1,param_2 + 0x28);
          *(undefined1 *)(param_1 + 6) = 1;
          return 1;
        }
      }
    }
  }
  return 0;
}

