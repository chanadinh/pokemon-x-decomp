/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10824978(int param_1,uint *param_2)

{
  byte bVar1;
  undefined1 uVar2;
  uint uVar3;
  ushort uVar4;
  ushort uVar5;
  undefined4 *puVar6;
  undefined4 uVar7;
  int iVar8;
  int iVar9;
  bool bVar10;
  
  bVar1 = *(byte *)((int)param_2 + 6);
  iVar8 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar8 = iVar8 + (uint)bVar1 * 4;
  if (*(int *)(iVar8 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar9 = *(int *)(iVar8 + 0x84);
  uVar4 = FUN_10874420(iVar9,0x10);
  uVar4 = uVar4 & 0xff;
  iVar8 = FUN_1086fe6c(uVar4,&DAT_108d10f0,2);
  if ((iVar8 == 0) && (iVar8 = FUN_10874420(iVar9,0xd), iVar8 != 0)) {
    uVar5 = (ushort)*(byte *)((int)param_2 + 7);
    bVar10 = uVar5 == 0;
    if (bVar10) {
      uVar5 = (ushort)param_2[1];
    }
    if (!bVar10 || uVar5 != uVar4) {
      if ((*param_2 & 0x800000) != 0) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,(*param_2 & 0x1fff) >> 8);
      }
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x53,*(undefined1 *)((int)param_2 + 6),
                   (short)param_2[1]);
      FUN_1086bbe8(param_1,param_2 + 3);
      puVar6 = (undefined4 *)(param_1 + 0x437c);
      uVar7 = FUN_1086b284();
      uVar2 = *(undefined1 *)((int)param_2 + 6);
      uVar3 = param_2[1];
      FUN_1086bb84();
      FUN_1086bac0(2,uVar2);
      FUN_1086bac0(0xf,uVar4);
      FUN_1086bac0(0x10,(short)uVar3);
      FUN_1086b9a4(param_1,0x93);
      FUN_1086b880();
      *puVar6 = uVar7;
      FUN_108679ac(iVar9);
      *(ushort *)(iVar9 + 0x14e) = (ushort)param_2[1] & 0xff;
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x20,*(undefined1 *)((int)param_2 + 6),
                   (short)param_2[1]);
      FUN_108678f8(iVar9);
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)((int)param_2 + 6));
      if ((ushort)param_2[1] != uVar4) {
        uVar7 = FUN_1086b284(puVar6);
        uVar2 = *(undefined1 *)((int)param_2 + 6);
        FUN_1086bb84();
        FUN_1086bac0(2,uVar2);
        FUN_1086b9a4(param_1,0x94);
        FUN_1086b880();
        *puVar6 = uVar7;
      }
      if ((*param_2 & 0x800000) != 0) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,(*param_2 & 0x1fff) >> 8);
      }
      FUN_108677e4(param_1,iVar9,uVar4,(ushort)param_2[1] & 0xff);
      return 1;
    }
  }
  return 0;
}

