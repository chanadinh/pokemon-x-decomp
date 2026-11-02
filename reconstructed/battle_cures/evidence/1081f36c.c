/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081f36c(int param_1,uint *param_2)

{
  byte bVar1;
  undefined2 uVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  
  bVar1 = *(byte *)((int)param_2 + 6);
  iVar6 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar6 = iVar6 + (uint)bVar1 * 4;
  if (*(int *)(iVar6 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar6 = *(int *)(iVar6 + 0x84);
  puVar7 = (undefined4 *)(param_1 + 0x437c);
  if ((uint)*(byte *)((int)param_2 + 6) != (*param_2 & 0x1fff) >> 8) {
    uVar4 = FUN_1086b284(puVar7);
    uVar3 = param_2[1];
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(iVar6 + 0x19));
    FUN_1086bac0(0x2e,(short)uVar3);
    FUN_1086b9fc(0x42,0);
    FUN_1086b9a4(param_1,0xa6);
    iVar5 = FUN_1086b91c(0x42);
    FUN_1086b880();
    *puVar7 = uVar4;
    if (iVar5 != 0) {
      uVar4 = FUN_1086b284(puVar7);
      FUN_1086bb84();
      FUN_1086bac0(2,*(undefined1 *)(iVar6 + 0x19));
      FUN_1086b9a4(param_1,0xa7);
      FUN_1086b880();
      *puVar7 = uVar4;
      return 0;
    }
  }
  if ((*param_2 & 0x800000) != 0) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,(*param_2 & 0x1fff) >> 8);
  }
  uVar2 = *(undefined2 *)(iVar6 + 0x12);
  FUN_1086bbe8(param_1,param_2 + 3);
  if (*(char *)((int)param_2 + 7) != '\0') {
    *(undefined2 *)(iVar6 + 0x14) = 0;
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x2e,*(undefined1 *)((int)param_2 + 6));
  }
  FUN_1086b140(param_1,iVar6,(short)param_2[1],0);
  if (((short)param_2[1] == 0) && (*(char *)((int)param_2 + 10) != '\0')) {
    uVar4 = FUN_1086b284(puVar7);
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(iVar6 + 0x19));
    FUN_1086bac0(0x2e,uVar2);
    FUN_1086bac0(0x52,1);
    FUN_1086b9a4(param_1,0x9b);
    FUN_1086b880();
    *puVar7 = uVar4;
  }
  if ((*param_2 & 0x800000) != 0) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,(*param_2 & 0x1fff) >> 8);
  }
  if ((char)param_2[2] != '\0') {
    bVar1 = *(byte *)((int)param_2 + 9);
    iVar5 = *(int *)(param_1 + 8);
    if (0x17 < bVar1) {
      FUN_10800394(0,0);
    }
    iVar5 = iVar5 + (uint)bVar1 * 4;
    if (*(int *)(iVar5 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    *(undefined2 *)(*(int *)(iVar5 + 0x84) + 0x14) = 0;
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x2e,*(undefined1 *)((int)param_2 + 9));
  }
  if (*(short *)(iVar6 + 0x12) != 0) {
    uVar4 = FUN_1086b284(puVar7);
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(iVar6 + 0x19));
    FUN_1086bac0(0x2f,0xf);
    FUN_1086b9a4(param_1,0x9c);
    FUN_1086b880();
    *puVar7 = uVar4;
  }
  return 1;
}

