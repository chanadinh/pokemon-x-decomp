/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

byte FUN_10825eb8(int param_1,int param_2)

{
  byte bVar1;
  char cVar2;
  undefined1 uVar3;
  int iVar4;
  undefined8 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  int iVar8;
  undefined4 uVar9;
  int iVar10;
  int *piVar11;
  bool bVar12;
  undefined8 uVar13;
  undefined1 auStack_70 [20];
  int local_5c;
  uint local_3c;
  
  bVar1 = *(byte *)(param_2 + 4);
  iVar7 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar7 = iVar7 + (uint)bVar1 * 4;
  if (*(int *)(iVar7 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar7 = *(int *)(iVar7 + 0x84);
  bVar1 = *(byte *)(param_2 + 5);
  iVar8 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar8 = iVar8 + (uint)bVar1 * 4;
  if (*(int *)(iVar8 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  uVar9 = *(undefined4 *)(iVar8 + 0x84);
  FUN_10869c5c(param_1,*(undefined2 *)(param_2 + 8),iVar7,auStack_70);
  FUN_10869870(*(undefined4 *)(param_1 + 0x1260));
  FUN_10869870(*(undefined4 *)(param_1 + 0x1268));
  FUN_108696b0(*(undefined4 *)(param_1 + 0x1268),uVar9);
  *(undefined1 *)(*(int *)(param_1 + 0x1268) + 0x71) = 1;
  iVar8 = *(int *)(param_1 + 0x1268);
  *(undefined1 *)(iVar8 + 0x70) = 0;
  local_5c = param_1 + 0x120c;
  while( true ) {
    bVar1 = *(byte *)(iVar8 + 0x70);
    if ((uint)*(byte *)(iVar8 + 0x6e) <= (uint)bVar1) break;
    *(byte *)(iVar8 + 0x70) = bVar1 + 1;
    iVar10 = *(int *)(iVar8 + (uint)bVar1 * 4);
    if (iVar10 == 0) break;
    iVar4 = FUN_10874420(iVar10,0xd);
    if ((iVar4 == 0) ||
       (iVar4 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)(iVar10 + 0x19)), iVar4 == 10)) {
      FUN_10869784(iVar8,iVar10);
    }
  }
  iVar8 = *(int *)(param_1 + 0x1268);
  cVar2 = *(char *)(iVar8 + 0x6f);
  bVar12 = cVar2 == '\0';
  if (bVar12) {
    cVar2 = *(char *)(iVar8 + 0x71);
  }
  if ((bVar12 && cVar2 == '\0') || (*(char *)(iVar8 + 0x6e) != '\0')) {
    FUN_1082399c(param_1,auStack_70,iVar7);
    iVar10 = param_1 + 0x4360;
    FUN_10826800(param_1,auStack_70,iVar7,*(undefined4 *)(param_1 + 0x1268),iVar10);
    FUN_10863790(param_1,auStack_70,iVar7,*(undefined4 *)(param_1 + 0x1268),iVar10);
    FUN_10863604(param_1,auStack_70,iVar7,*(undefined4 *)(param_1 + 0x1268));
    iVar8 = *(int *)(param_1 + 0x1268);
    cVar2 = *(char *)(iVar8 + 0x6f);
    bVar12 = cVar2 == '\0';
    if (bVar12) {
      cVar2 = *(char *)(iVar8 + 0x71);
    }
    if ((bVar12 && cVar2 == '\0') || (*(char *)(iVar8 + 0x6e) != '\0')) {
      puVar5 = *(undefined8 **)(param_1 + 0xdb4);
      uVar13 = *puVar5;
      uVar9 = *(undefined4 *)(puVar5 + 1);
      *(undefined2 *)puVar5 = 0;
      *(undefined1 *)((int)puVar5 + 2) = 10;
      *(undefined1 *)((int)puVar5 + 3) = 10;
      *(undefined1 *)((int)puVar5 + 4) = 0;
      *(byte *)((int)puVar5 + 5) = *(byte *)((int)puVar5 + 5) & 0xfc;
      *(undefined4 *)(puVar5 + 1) = 0;
      piVar11 = *(int **)(param_1 + 0x1268);
      iVar4 = *(int *)(param_1 + 0xdb4);
      local_3c = (uint)*(byte *)((int)piVar11 + 0x6f);
      iVar8 = local_5c;
      uVar3 = FUN_1086c2cc(local_5c,*(undefined1 *)(iVar7 + 0x19));
      *(undefined1 *)(iVar4 + 2) = uVar3;
      *(undefined1 *)(iVar4 + 3) = 10;
      *(byte *)(iVar4 + 5) = *(byte *)(iVar4 + 5) & 0xfc;
      *(undefined4 *)(iVar4 + 8) = 0;
      if ((local_3c == 1) && ((*(char *)((int)piVar11 + 0x6e) != '\0' && (*piVar11 != 0)))) {
        uVar3 = FUN_1086c2cc(iVar8,*(undefined1 *)(*piVar11 + 0x19));
        *(undefined1 *)(iVar4 + 3) = uVar3;
      }
      *(undefined1 *)(*(int *)(param_1 + 0xdb4) + 2) = *(undefined1 *)(param_2 + 6);
      *(undefined1 *)(*(int *)(param_1 + 0xdb4) + 4) = 1;
      FUN_108225dc(param_1,auStack_70,iVar7,*(undefined4 *)(param_1 + 0x1268),iVar10,1);
      puVar6 = *(undefined4 **)(param_1 + 0xdb4);
      *puVar6 = (int)uVar13;
      puVar6[1] = (int)((ulonglong)uVar13 >> 0x20);
      puVar6[2] = uVar9;
      return *(byte *)((int)puVar6 + 5) & 1;
    }
  }
  else {
    FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x69,0x4a,0xffff0000);
  }
  return 0;
}

