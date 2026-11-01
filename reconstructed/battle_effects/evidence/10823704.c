/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_10823704(int param_1,int param_2)

{
  byte bVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  char cVar5;
  char cVar6;
  char cVar7;
  bool bVar8;
  int iVar9;
  int iVar10;
  
  bVar1 = *(byte *)(param_2 + 4);
  iVar10 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar10 = iVar10 + (uint)bVar1 * 4;
  if (*(int *)(iVar10 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar9 = *(int *)(iVar10 + 0x84);
  iVar10 = FUN_10874420(iVar9,0xd);
  bVar8 = false;
  if (iVar10 != 0) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0xd,*(undefined1 *)(param_2 + 4));
    cVar2 = *(char *)(iVar9 + 0x104);
    if (cVar2 < '\x06') {
      *(char *)(iVar9 + 0x104) = '\x06';
    }
    cVar3 = *(char *)(iVar9 + 0x105);
    if (cVar3 < '\x06') {
      *(undefined1 *)(iVar9 + 0x105) = 6;
    }
    cVar4 = *(char *)(iVar9 + 0x106);
    if (cVar4 < '\x06') {
      *(undefined1 *)(iVar9 + 0x106) = 6;
    }
    cVar5 = *(char *)(iVar9 + 0x107);
    if (cVar5 < '\x06') {
      *(undefined1 *)(iVar9 + 0x107) = 6;
    }
    cVar6 = *(char *)(iVar9 + 0x108);
    if (cVar6 < '\x06') {
      *(undefined1 *)(iVar9 + 0x108) = 6;
    }
    cVar7 = *(char *)(iVar9 + 0x109);
    if (cVar7 < '\x06') {
      *(undefined1 *)(iVar9 + 0x109) = 6;
    }
    bVar8 = cVar7 < '\x06' ||
            (cVar6 < '\x06' ||
            (cVar5 < '\x06' || (cVar4 < '\x06' || (cVar3 < '\x06' || cVar2 < '\x06'))));
    if (*(char *)(iVar9 + 0x10a) < '\x06') {
      bVar8 = true;
      *(undefined1 *)(iVar9 + 0x10a) = 6;
    }
  }
  return bVar8;
}

