/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10869870(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = 0;
  iVar2 = 10;
  do {
    iVar3 = param_1 + iVar1 * 2;
    *(undefined4 *)(param_1 + iVar1 * 4) = 0;
    *(undefined2 *)(iVar3 + 0x28) = 0;
    *(undefined2 *)(iVar3 + 0x3c) = 0;
    *(undefined1 *)(param_1 + iVar1 + 0x50) = 0;
    iVar2 = iVar2 + -1;
    iVar1 = iVar1 + 1;
    *(undefined2 *)(iVar3 + 0x5a) = 0;
  } while (iVar2 != 0);
  *(undefined1 *)(param_1 + 0x6e) = 0;
  *(undefined1 *)(param_1 + 0x6f) = 0;
  *(undefined1 *)(param_1 + 0x70) = 0;
  *(undefined1 *)(param_1 + 0x71) = 0;
  return;
}

