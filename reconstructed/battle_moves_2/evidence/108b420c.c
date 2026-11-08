/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b420c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x3d), iVar1 == 0xb)) {
    iVar1 = FUN_1086b91c(0x3e);
    FUN_108658d4(0x3e,iVar1 + 1);
    return;
  }
  return;
}

