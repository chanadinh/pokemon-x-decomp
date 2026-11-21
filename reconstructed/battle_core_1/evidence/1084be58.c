/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_1084be58(int param_1)

{
  int iVar1;
  
  FUN_1086bb84();
  FUN_1086b9fc(0x42,0);
  FUN_1086b9a4(param_1,0x83);
  iVar1 = FUN_1086b91c(0x42);
  if (iVar1 != 0) {
    FUN_1086b880();
    return 0;
  }
  FUN_1086b880();
  return *(undefined1 *)(*(int *)(param_1 + 0x1c) + 1);
}

