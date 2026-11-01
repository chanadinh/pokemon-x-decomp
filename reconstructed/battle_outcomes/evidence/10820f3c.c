/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10820f3c(int param_1,int param_2)

{
  byte bVar1;
  ushort uVar2;
  ushort uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  
  if (*(char *)(param_2 + 4) != *(char *)(param_2 + 5)) {
    uVar4 = FUN_1087425c();
    uVar5 = FUN_1087425c(*(undefined1 *)(param_2 + 5));
    if (uVar5 == uVar4) {
      bVar1 = *(byte *)(param_2 + 4);
      iVar6 = *(int *)(param_1 + 8);
      if (0x17 < bVar1) {
        FUN_10800394(0,0);
      }
      iVar6 = iVar6 + (uint)bVar1 * 4;
      if (*(int *)(iVar6 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      uVar7 = *(undefined4 *)(iVar6 + 0x84);
      bVar1 = *(byte *)(param_2 + 5);
      iVar6 = *(int *)(param_1 + 8);
      if (0x17 < bVar1) {
        FUN_10800394(0,0);
      }
      iVar6 = iVar6 + (uint)bVar1 * 4;
      if (*(int *)(iVar6 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      uVar8 = *(undefined4 *)(iVar6 + 0x84);
      iVar6 = FUN_10874420(uVar7,0xd);
      if ((iVar6 != 0) && (iVar6 = FUN_10874420(uVar8,0xd), iVar6 != 0)) {
        iVar6 = *(int *)(param_1 + 8);
        if (3 < uVar4) {
          FUN_10800394(0,0);
        }
        iVar6 = iVar6 + uVar4 * 0x1c + 4;
        uVar2 = FUN_1086fc8c(iVar6,uVar7);
        uVar3 = FUN_1086fc8c(iVar6,uVar8);
        if (-1 < (short)uVar2 && -1 < (short)uVar3) {
          FUN_10867e74(param_1,uVar4,uVar2 & 0xff,uVar3 & 0xff,0);
          FUN_1086bbe8(param_1,param_2 + 8);
          FUN_10867d64(param_1,uVar4,uVar2 & 0xff,uVar3 & 0xff);
          return 1;
        }
      }
    }
  }
  return 0;
}

