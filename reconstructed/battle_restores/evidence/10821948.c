/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_10821948(int param_1,uint *param_2,int param_3)

{
  uint uVar1;
  byte bVar2;
  int iVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  bool bVar7;
  bool bVar8;
  
  iVar5 = *(int *)(param_1 + 8);
  uVar1 = (*param_2 & 0x1fff) >> 8;
  if (0x17 < uVar1) {
    FUN_10800394(0,0);
  }
  iVar5 = iVar5 + uVar1 * 4;
  if (*(int *)(iVar5 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar6 = *(int *)(iVar5 + 0x84);
  bVar2 = *(byte *)((int)param_2 + 6);
  iVar5 = *(int *)(param_1 + 8);
  if (0x17 < bVar2) {
    FUN_10800394(0,0);
  }
  iVar5 = iVar5 + (uint)bVar2 * 4;
  if (*(int *)(iVar5 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  puVar4 = *(undefined4 **)(iVar5 + 0x84);
  iVar5 = thunk_FUN_004a1658(*puVar4,2);
  if (iVar5 != 0) {
    return false;
  }
  iVar5 = FUN_10874420(puVar4,0xd);
  if (iVar5 == 0) {
    return false;
  }
  iVar5 = FUN_10874420(puVar4,0xd);
  iVar3 = FUN_10874420(puVar4,0xe);
  if (iVar5 == iVar3) {
    return false;
  }
  if ((*param_2 & 0x800000) != 0) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar6 + 0x19));
  }
  bVar7 = *(char *)((int)param_2 + 7) == '\0';
  bVar8 = !bVar7;
  if (bVar7) {
    bVar8 = (puVar4[0x16] & 7) != 0;
    if (bVar8) {
      FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x37a,*(undefined1 *)((int)puVar4 + 0x19),
                   0xffff0000);
      *(byte *)(param_1 + 0x1093) = *(byte *)(param_1 + 0x1093) | 0x20;
    }
    bVar8 = !bVar8;
    if (!bVar8) goto LAB_10821b48;
  }
  uVar1 = param_2[1];
  iVar5 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)((int)puVar4 + 0x19));
  if (iVar5 != 10) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x57,iVar5,0x29e);
  }
  FUN_10865788(param_1,puVar4,(short)uVar1,1);
  if ((*(ushort *)((int)param_2 + 10) & 0x7f) == 0) {
    if (param_3 != 0) {
      FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x392,*(undefined1 *)((int)param_2 + 6),
                   param_3,0xffff0000);
    }
  }
  else {
    FUN_1086bbe8(param_1,param_2 + 2);
  }
LAB_10821b48:
  if ((*param_2 & 0x800000) != 0) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar6 + 0x19));
  }
  return bVar8;
}

