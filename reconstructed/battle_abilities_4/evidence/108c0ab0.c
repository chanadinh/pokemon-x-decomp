/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0ab0(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_1086b91c(0x27);
    FUN_108658d4(0x27,(uVar2 & 0x7fff) << 1);
    return;
  }
  return;
}

