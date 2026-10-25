/* Unreviewed Ghidra pseudocode obtained through live REA MCP; types require assembly review. */

void move_cache_store(int param_1,int param_2)

{
  undefined1 extraout_r1;
  uint uVar1;
  
  uVar1 = 0;
  if (*(char *)(param_1 + 8) != '\0') {
    do {
      if (*(short *)(*(int *)(*(int *)(param_1 + 4) + uVar1 * 4) + 4) == *(short *)(param_2 + 4)) {
        return;
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < *(byte *)(param_1 + 8));
  }
  FUN_0017c5d8(param_2,*(undefined4 *)(*(int *)(param_1 + 4) + (uint)*(byte *)(param_1 + 9) * 4));
  FUN_0015e958(*(byte *)(param_1 + 9) + 1,*(undefined1 *)(param_1 + 8));
  *(undefined1 *)(param_1 + 9) = extraout_r1;
  return;
}

