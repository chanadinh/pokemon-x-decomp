/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081e0f8(int param_1,int param_2)

{
  byte bVar1;
  byte bVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  ushort uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  bool bVar10;
  
  bVar1 = *(byte *)(param_2 + 4);
  iVar9 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar9 = iVar9 + (uint)bVar1 * 4;
  if (*(int *)(iVar9 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar9 = *(int *)(iVar9 + 0x84);
  uVar5 = *(short *)(param_2 + 6) + *(short *)(iVar9 + 0x10);
  *(ushort *)(iVar9 + 0x10) = uVar5;
  if (*(ushort *)(iVar9 + 0xe) < uVar5) {
    *(ushort *)(iVar9 + 0x10) = *(ushort *)(iVar9 + 0xe);
  }
  FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),3,*(undefined1 *)(param_2 + 4),
               *(undefined2 *)(param_2 + 6));
  bVar1 = *(byte *)(param_2 + 4);
  uVar8 = (uint)bVar1;
  if (uVar8 < 0x18) {
    iVar9 = param_1 + 0xca4 + uVar8;
    if (*(char *)(iVar9 + 0xc4) == '\0') goto LAB_1081e204;
    uVar6 = 0;
    *(undefined1 *)(iVar9 + 0xc4) = 0;
    do {
      uVar7 = param_1 + 0xca4 + uVar6;
      bVar10 = *(byte *)(uVar7 + 0xf4) == uVar8;
      if (!bVar10) {
        uVar7 = (uint)*(byte *)(uVar7 + 0xf5);
      }
      if (bVar10 || uVar7 == uVar8) goto LAB_1081e204;
      uVar6 = uVar6 + 2;
    } while (uVar6 < 0x18);
    bVar2 = *(byte *)(param_1 + 0xdb0);
    if (bVar2 < 0x18) {
      *(byte *)(param_1 + 0xdb0) = bVar2 + 1;
      *(byte *)((uint)bVar2 + param_1 + 0xd98) = bVar1;
      goto LAB_1081e204;
    }
  }
  FUN_10800394(0,0);
LAB_1081e204:
  FUN_1086bbe8(param_1,param_2 + 8);
  iVar9 = FUN_10873164(*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),
                       *(undefined1 *)(param_2 + 4));
  if (iVar9 != 10) {
    uVar3 = FUN_1087425c(*(undefined1 *)(param_2 + 4));
    uVar4 = FUN_10871b10(*(undefined4 *)(param_1 + 4),iVar9);
    FUN_1086c720(param_1,uVar3,uVar4);
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x44,uVar3,uVar4,uVar4,1);
    FUN_1086c500(param_1);
  }
  return 1;
}

