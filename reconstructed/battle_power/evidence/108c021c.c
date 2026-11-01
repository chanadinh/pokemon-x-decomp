/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c021c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (uVar2 = FUN_1086b91c(0x31), uVar2 < 0x3d)) {
    FUN_1084ce84(0x32,0x1800);
    return;
  }
  return;
}

