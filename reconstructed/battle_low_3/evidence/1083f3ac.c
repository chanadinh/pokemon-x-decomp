/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1083f3ac(int param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 1;
  iVar3 = 0;
  if (DAT_108fcb84 != 0xd8b) {
    DAT_108fcb84 = 0xd8b;
  }
  do {
    cVar1 = *(char *)(param_1 + iVar3 + 0x6ca);
    if ((cVar1 != -1) &&
       (iVar2 = FUN_1083be10(param_1,cVar1,*(int *)(param_1 + 0x6b4) + iVar3 * -10), iVar2 == 0)) {
      iVar4 = 0;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 < 6);
  *(int *)(param_1 + 0x6b4) = *(int *)(param_1 + 0x6b4) + 1;
  if (iVar4 != 0) {
    FUN_10859cc4(*(int *)(param_1 + 400) + 0x280);
  }
  return iVar4;
}

