/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086fd8c(int param_1,uint param_2)

{
  uint uVar1;
  
  if (5 < param_2) {
    *(undefined4 *)(param_1 + param_2 * 4 + 0x1c) = 0;
    *(undefined1 *)(param_1 + param_2 + 0xb4) = 0;
    return;
  }
  uVar1 = 1;
  do {
    *(undefined4 *)(param_1 + uVar1 * 4 + 0x1c) = 0;
    *(undefined1 *)(param_1 + uVar1 + 0xb4) = 0;
    if ((uVar1 & 0xffff) == 2) {
      *(undefined4 *)(param_1 + 0x40) = 0;
      *(undefined1 *)(param_1 + 0xbd) = 0;
    }
    uVar1 = uVar1 + 1;
  } while (uVar1 < 6);
  return;
}

