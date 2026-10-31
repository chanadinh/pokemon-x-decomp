/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1084d370(int param_1)

{
  char cVar1;
  int iVar2;
  bool bVar3;
  
  if (param_1 != 0) {
    for (iVar2 = *(int *)(param_1 + 4); iVar2 != 0; iVar2 = *(int *)(iVar2 + 4)) {
      cVar1 = *(char *)(iVar2 + 0x10);
      bVar3 = cVar1 == *(char *)(param_1 + 0x10);
      if (bVar3) {
        cVar1 = *(char *)(iVar2 + 0x3a);
      }
      if ((bVar3 && cVar1 == *(char *)(param_1 + 0x3a)) &&
         ((*(uint *)(iVar2 + 0x18) & 0x8000000) == 0)) {
        return iVar2;
      }
    }
  }
  return 0;
}

