/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108afc4c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1084be58(param_2), iVar1 == 1)) {
    iVar1 = FUN_1086b91c(0x21);
    if (iVar1 == 1) {
      iVar1 = 2;
    }
    FUN_108658d4(0x21,iVar1);
    return;
  }
  return;
}

