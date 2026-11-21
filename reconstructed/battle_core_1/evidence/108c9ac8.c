/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_108c9ac8(int *param_1,uint param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 0;
  iVar2 = *param_1;
  while( true ) {
    iVar2 = iVar2 + -1;
    if (iVar2 < 0) {
      iVar2 = 399;
    }
    if (*param_1 == iVar2) break;
    if (*(ushort *)(param_1 + iVar2 * 2 + 2) == 0) {
      return iVar1;
    }
    if ((*(ushort *)(param_1 + iVar2 * 2 + 2) == param_2) && (param_1[iVar2 * 2 + 1] == param_3)) {
      iVar1 = iVar1 + 1;
    }
  }
  return iVar1;
}

