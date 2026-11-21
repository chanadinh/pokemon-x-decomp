/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083cbb0(int param_1)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x145e) {
    DAT_108fcb84 = 0x145e;
  }
  iVar1 = *(int *)(param_1 + 400);
  *(undefined4 *)(iVar1 + 0x23c) = 0;
  *(undefined1 *)(iVar1 + 0x244) = 0;
  FUN_108568ec(0);
  FUN_1085a18c(*(undefined4 *)(iVar1 + 0x238),1);
  (**(code **)(**(int **)(iVar1 + 0x88) + 0x10))(*(int **)(iVar1 + 0x88),0);
  *(undefined4 *)(iVar1 + 0x88) = 0;
  *(undefined1 *)(iVar1 + 0x93) = 0;
  FUN_108568ec(0);
  FUN_1085a18c(*(undefined4 *)(iVar1 + 0x74),1);
  return;
}

