/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6adc(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  uVar1 = FUN_1086b91c(2);
  iVar2 = FUN_1084c5a8(uVar1,param_3);
  if (iVar2 != 0) {
    FUN_108658d4(0x53,1);
    return;
  }
  return;
}

