/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108be2fc(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(4);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(0x39);
    iVar2 = FUN_1089e58c(uVar1);
    if (iVar2 == 2) {
      FUN_1084ce84(0x36,0xc00);
      return;
    }
  }
  return;
}

