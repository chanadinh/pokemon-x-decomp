/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10824774(int param_1,uint *param_2)

{
  byte bVar1;
  undefined1 uVar2;
  byte bVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  int iVar7;
  undefined4 *puVar8;
  
  iVar7 = *(int *)(param_1 + 8);
  uVar6 = (*param_2 & 0x1fff) >> 8;
  if (0x17 < uVar6) {
    FUN_10800394(0,0);
  }
  iVar7 = iVar7 + uVar6 * 4;
  if (*(int *)(iVar7 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  bVar3 = (byte)param_2[1];
  iVar7 = *(int *)(iVar7 + 0x84);
  puVar8 = (undefined4 *)(param_1 + 0x437c);
  if (bVar3 == 0) {
    if (*(char *)((int)param_2 + 6) == '\0') {
      return 0;
    }
    if ((*param_2 & 0x800000) != 0) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar7 + 0x19));
    }
    FUN_1086bbe8(param_1,param_2 + 2);
    uVar5 = FUN_1086b284(puVar8);
    FUN_1086bb84();
    FUN_1086b9a4(param_1,0x82);
    FUN_1086b880();
    *puVar8 = uVar5;
    uVar6 = *param_2;
  }
  else {
    if (4 < bVar3) {
      return 0;
    }
    bVar1 = *(byte *)(*(int *)(param_1 + 0x1c) + 1);
    if (bVar1 == bVar3) {
      if (*(char *)((int)param_2 + 5) != -1) {
        return 0;
      }
      if ((bVar1 != 0) && (*(int *)(*(int *)(param_1 + 0x1c) + 4) == 0xff)) {
        return 0;
      }
    }
    if ((*param_2 & 0x800000) != 0) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar7 + 0x19));
    }
    uVar2 = (undefined1)param_2[1];
    iVar4 = *(int *)(param_1 + 0x1c);
    bVar3 = *(byte *)((int)param_2 + 5);
    *(undefined1 *)(iVar4 + 1) = uVar2;
    *(uint *)(iVar4 + 4) = (uint)bVar3;
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x45,uVar2);
    uVar5 = FUN_1086b284(puVar8);
    FUN_1086bb84();
    FUN_1086bac0(0x3a,uVar2);
    FUN_1086b9fc(0x42,0);
    FUN_1086b9a4(param_1,0x86);
    iVar4 = FUN_1086b91c(0x42);
    if (iVar4 == 0) {
      FUN_1086b9a4(param_1,0x87);
    }
    FUN_1086b880();
    *puVar8 = uVar5;
    FUN_1086bbe8(param_1,param_2 + 2);
    uVar6 = *param_2;
  }
  if ((uVar6 & 0x800000) != 0) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar7 + 0x19));
  }
  return 1;
}

