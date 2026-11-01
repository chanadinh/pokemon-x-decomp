/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10822e28(int param_1,uint *param_2)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  int iVar11;
  bool bVar12;
  
  iVar4 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)((int)param_2 + 6));
  if (iVar4 != 10) {
    bVar1 = *(byte *)((int)param_2 + 6);
    iVar4 = *(int *)(param_1 + 8);
    if (0x17 < bVar1) {
      FUN_10800394(0,0);
    }
    iVar4 = iVar4 + (uint)bVar1 * 4;
    if (*(int *)(iVar4 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar11 = *(int *)(iVar4 + 0x84);
    iVar4 = FUN_10874420(iVar11,0xd);
    if ((iVar4 != 0) && (*(short *)(iVar11 + 0xc) != 0x1ed)) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x17,*(undefined1 *)((int)param_2 + 6),
                   (short)param_2[1]);
      uVar5 = FUN_10870994(iVar11);
      uVar6 = (uint)(ushort)param_2[1];
      bVar1 = *(byte *)(iVar11 + 0x100);
      bVar12 = true;
      uVar7 = uVar6 & 0x1f;
      bVar2 = *(byte *)(iVar11 + 0x101);
      bVar3 = *(byte *)(iVar11 + 0x102);
      *(char *)(iVar11 + 0x100) = (char)uVar7;
      *(byte *)(iVar11 + 0x101) = (byte)((uVar6 << 0x16) >> 0x1b);
      *(byte *)(iVar11 + 0x102) = (byte)((uVar6 << 0x11) >> 0x1b);
      if (((((uVar7 != bVar1) || ((uint)*(byte *)(iVar11 + 0x101) != (uint)bVar2)) &&
           ((uint)*(byte *)(iVar11 + 0x101) != (uint)bVar1 || uVar7 != bVar2)) ||
          (bVar12 = (uVar6 & 0x7fff) >> 10 != (uint)bVar3, bVar12)) && ((*param_2 & 0x800000) != 0))
      {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar11 + 0x19));
      }
      if (*(char *)((int)param_2 + 7) == '\0') {
        uVar6 = (uint)(ushort)param_2[1];
        uVar7 = 0xffff0000;
        if ((((uVar6 & 0x1f) == (uVar6 & 0x3ff) >> 5) && ((uVar6 & 0x7c00) == 0x4800)) &&
           (uVar6 != uVar5)) {
          uVar7 = (uint)*(byte *)((int)param_2 + 6);
          uVar8 = *(undefined4 *)(param_1 + 0xc);
          uVar10 = 899;
          uVar9 = 0x6a;
        }
        else {
          if ((char)param_2[2] == '\0') goto LAB_10822fd0;
          uVar8 = *(undefined4 *)(param_1 + 0xc);
          uVar10 = 0x4a;
          uVar9 = 0x69;
        }
        FUN_1086b5e0(uVar8,uVar9,uVar10,uVar7);
      }
LAB_10822fd0:
      if ((bVar12) && ((*param_2 & 0x800000) != 0)) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar11 + 0x19));
      }
      return 1;
    }
  }
  return 0;
}

