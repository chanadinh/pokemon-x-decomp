/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108c9a64(int *param_1,uint param_2,int param_3)

{
  int iVar1;
  
  iVar1 = *param_1;
  do {
    iVar1 = iVar1 + -1;
    if (iVar1 < 0) {
      iVar1 = 399;
    }
    if (*param_1 == iVar1) {
      return 0;
    }
    if (*(ushort *)(param_1 + iVar1 * 2 + 2) == 0) {
      return 0;
    }
  } while ((*(ushort *)(param_1 + iVar1 * 2 + 2) != param_2) || (param_1[iVar1 * 2 + 1] != param_3))
  ;
  return 1;
}

