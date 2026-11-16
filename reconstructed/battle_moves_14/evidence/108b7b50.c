/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7b50(int param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (uVar2 = FUN_1086b91c(0x12), uVar2 != *(ushort *)(param_1 + 0x38))) {
    *param_4 = 2;
  }
  return;
}

