/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a9268(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x16), iVar1 == 0)) {
    uVar2 = FUN_1084cbe4(param_1);
    FUN_1084ce84(0x32,uVar2);
    return;
  }
  return;
}

