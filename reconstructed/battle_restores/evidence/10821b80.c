/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10821b80(int param_1,uint *param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  undefined1 uVar2;
  byte bVar3;
  undefined4 uVar4;
  short *psVar5;
  undefined4 uVar6;
  byte bVar7;
  int iVar8;
  undefined4 *puVar9;
  uint uVar10;
  
  iVar8 = *(int *)(param_1 + 8);
  uVar10 = (*param_2 & 0x1fff) >> 8;
  if (0x17 < uVar10) {
    FUN_10800394(0,0,0,param_4,param_4);
  }
  if (*(int *)(iVar8 + uVar10 * 4 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  bVar3 = *(byte *)((int)param_2 + 5);
  iVar8 = *(int *)(param_1 + 8);
  if (0x17 < bVar3) {
    FUN_10800394(0,0);
  }
  iVar8 = iVar8 + (uint)bVar3 * 4;
  if (*(int *)(iVar8 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  puVar9 = *(undefined4 **)(iVar8 + 0x84);
  iVar8 = thunk_FUN_004a1658(*puVar9,2);
  if (((iVar8 == 0) && (iVar8 = FUN_10874420(puVar9,0xd), iVar8 != 0)) ||
     ((*(byte *)((int)param_2 + 7) & 2) != 0)) {
    uVar10 = (uint)*(byte *)((int)param_2 + 6);
    bVar3 = ~*(byte *)((int)param_2 + 7) & 1;
    if (bVar3 == 0) {
      iVar8 = uVar10 * 0xe + 0x11c;
    }
    else {
      iVar8 = uVar10 * 0xe + 0x116;
    }
    psVar5 = (short *)((int)puVar9 + iVar8);
    if ((*psVar5 == 0) || ((char)psVar5[1] != *(char *)((int)psVar5 + 3))) {
      cVar1 = (char)param_2[1];
      uVar2 = *(undefined1 *)((int)puVar9 + 0x19);
      if (bVar3 == 0) {
        if (3 < uVar10) {
          FUN_10800394(0,0);
        }
        bVar3 = *(byte *)((int)puVar9 + uVar10 * 0xe + 0x11f);
        bVar7 = *(char *)((int)puVar9 + uVar10 * 0xe + 0x11e) + cVar1;
        *(byte *)((int)puVar9 + uVar10 * 0xe + 0x11e) = bVar7;
        if (bVar3 < bVar7) {
          *(byte *)((int)puVar9 + uVar10 * 0xe + 0x11e) = bVar3;
        }
        if (*(char *)((int)puVar9 + uVar10 * 0xe + 0x122) != '\0') {
          *(undefined1 *)((int)puVar9 + uVar10 * 0xe + 0x118) =
               *(undefined1 *)((int)puVar9 + uVar10 * 0xe + 0x11e);
        }
        uVar4 = *(undefined4 *)(param_1 + 0xc);
        uVar6 = 8;
      }
      else {
        if (3 < uVar10) {
          FUN_10800394(0,0);
        }
        bVar3 = *(byte *)((int)puVar9 + uVar10 * 0xe + 0x119);
        bVar7 = *(char *)((int)puVar9 + uVar10 * 0xe + 0x118) + cVar1;
        *(byte *)((int)puVar9 + uVar10 * 0xe + 0x118) = bVar7;
        if (bVar3 < bVar7) {
          *(byte *)((int)puVar9 + uVar10 * 0xe + 0x118) = bVar3;
        }
        if (*(char *)((int)puVar9 + uVar10 * 0xe + 0x122) != '\0') {
          *(undefined1 *)((int)puVar9 + uVar10 * 0xe + 0x11e) =
               *(undefined1 *)((int)puVar9 + uVar10 * 0xe + 0x118);
        }
        uVar4 = *(undefined4 *)(param_1 + 0xc);
        uVar6 = 9;
      }
      FUN_1086c22c(uVar4,uVar6,uVar2,uVar10,cVar1);
      FUN_1086bbe8(param_1,param_2 + 2);
      return 1;
    }
  }
  return 0;
}

