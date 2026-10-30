/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_00175fec(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = 0;
  if (*(char *)(param_1 + 8) != '\0') {
    do {
      if (*(int *)(*(int *)(*(int *)(param_1 + 4) + iVar1 * 4) + 4) == param_2) {
        FUN_0017c5b0(*(undefined4 *)(*(int *)(param_1 + 4) + iVar1 * 4),param_3);
        return 1;
      }
      iVar1 = iVar1 + 1;
    } while (iVar1 < (int)(uint)*(byte *)(param_1 + 8));
  }
  return 0;
}

