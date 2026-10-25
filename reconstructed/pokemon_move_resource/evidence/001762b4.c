/* Unreviewed Ghidra pseudocode obtained through live REA MCP; types require assembly review. */

undefined4 move_cache_lookup(int param_1,uint param_2,undefined4 param_3)

{
  uint uVar1;
  
  uVar1 = 0;
  if (*(char *)(param_1 + 8) != '\0') {
    do {
      if (*(ushort *)(*(int *)(*(int *)(param_1 + 4) + uVar1 * 4) + 4) == param_2) {
        FUN_0017c5d8(*(undefined4 *)(*(int *)(param_1 + 4) + uVar1 * 4),param_3);
        return 1;
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < *(byte *)(param_1 + 8));
  }
  return 0;
}

