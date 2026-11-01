/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081db1c(int param_1,uint *param_2)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  
  uVar5 = param_2[1];
  iVar8 = *(int *)(param_1 + 8);
  if (0x17 < (byte)uVar5) {
    FUN_10800394(0,0);
  }
  iVar8 = iVar8 + (uint)(byte)uVar5 * 4;
  if (*(int *)(iVar8 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar7 = *(int *)(iVar8 + 0x84);
  iVar8 = FUN_10874420(iVar7,0xd);
  if ((iVar8 != 0) && (iVar8 = FUN_108abf28(iVar7,*(undefined2 *)((int)param_2 + 6)), iVar8 != 0)) {
    bVar1 = false;
    uVar3 = FUN_10863e4c(*(undefined4 *)(param_1 + 0xc),0x49,0x6e,0);
    uVar4 = FUN_1086b284();
    uVar5 = *param_2;
    FUN_1086bb84();
    FUN_1086b520(2,*(undefined1 *)(iVar7 + 0x19));
    FUN_1086b520(3,(uVar5 & 0x1fff) >> 8);
    DAT_108fca68 = DAT_108fca68 + 1;
    FUN_10864e98(param_1,0x7c,7,0);
    DAT_108fca68 = DAT_108fca68 + -1;
    iVar2 = DAT_108fca60;
    if (DAT_108fca68 == 0) {
      for (; iVar2 != 0; iVar2 = *(int *)(iVar2 + 4)) {
        *(undefined2 *)(iVar2 + 0x18) = 0;
      }
    }
    FUN_1086b880();
    if (((*(uint *)(param_1 + 0x437c) & 0x40000000) != 0) &&
       ((*(uint *)(param_1 + 0x437c) & 0x20000000) != 0)) {
      bVar1 = true;
      uVar6 = thunk_FUN_0039f0bc(*(undefined2 *)((int)param_2 + 6));
      FUN_108659b8(*(undefined4 *)(param_1 + 0xc),uVar3,0x49,(char)param_2[1],uVar6);
    }
    *(undefined4 *)(param_1 + 0x437c) = uVar4;
    thunk_FUN_1084d1cc(iVar8);
    if ((*(char *)((int)param_2 + 5) != '\0') &&
       (iVar8 = thunk_FUN_0039f0bc(*(undefined2 *)((int)param_2 + 6)), iVar8 != 0)) {
      *(byte *)(iVar7 + 0x16c) = *(byte *)(iVar7 + 0x16c) | 1;
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x1f,*(undefined1 *)(iVar7 + 0x19),0);
    }
    if (bVar1) {
      FUN_1086b098(param_1,iVar7,*(undefined2 *)((int)param_2 + 6),1);
    }
    return 1;
  }
  return 0;
}

