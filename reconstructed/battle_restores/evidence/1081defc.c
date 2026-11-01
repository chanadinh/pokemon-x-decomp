/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081defc(int param_1,uint *param_2)

{
  uint uVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  char cVar4;
  char cVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  int iVar9;
  
  iVar8 = *(int *)(param_1 + 8);
  uVar1 = (*param_2 & 0x1fff) >> 8;
  if (0x17 < uVar1) {
    FUN_10800394(0,0);
  }
  iVar8 = iVar8 + uVar1 * 4;
  if (*(int *)(iVar8 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  uVar1 = param_2[1];
  iVar8 = *(int *)(iVar8 + 0x84);
  iVar9 = *(int *)(param_1 + 8);
  if (0x17 < (byte)uVar1) {
    FUN_10800394(0,0);
  }
  iVar9 = iVar9 + (uint)(byte)uVar1 * 4;
  if (*(int *)(iVar9 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar9 = *(int *)(iVar9 + 0x84);
  if ((((*(byte *)(iVar8 + 0x1b) & 0x40) == 0) && ((*(byte *)(iVar9 + 0x1b) & 0x40) == 0)) &&
     (iVar6 = FUN_10874348(iVar8), iVar6 == 0x12)) {
    cVar4 = FUN_10874420(iVar8,0x10);
    iVar6 = FUN_1082a720(iVar8,iVar9);
    if (iVar6 != 0) {
      uVar2 = *(undefined1 *)(iVar8 + 0x19);
      uVar3 = *(undefined1 *)(iVar9 + 0x19);
      if ((*param_2 & 0x800000) != 0) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,uVar2);
      }
      FUN_108ae5e0(iVar8);
      FUN_108679ac(iVar8);
      FUN_108678f8(iVar8);
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x5f,uVar2,uVar3);
      FUN_1086bbe8(param_1,param_2 + 2);
      if ((*param_2 & 0x800000) != 0) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar8 + 0x19));
      }
      cVar5 = FUN_10874420(iVar8,0x11);
      if (cVar4 != cVar5) {
        uVar2 = *(undefined1 *)(iVar8 + 0x19);
        uVar7 = FUN_1086b284();
        FUN_1086bb84();
        FUN_1086bac0(2,uVar2);
        FUN_1086b9a4(param_1,0x94);
        FUN_1086b880();
        *(undefined4 *)(param_1 + 0x437c) = uVar7;
        FUN_108677e4(param_1,iVar8,cVar4,cVar5);
      }
      return 1;
    }
  }
  return 0;
}

