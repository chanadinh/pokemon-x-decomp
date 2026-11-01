/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bce68(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(4);
  if ((iVar2 == param_3) && (sVar1 = FUN_1086b91c(0x1e), sVar1 == 7)) {
    iVar2 = FUN_108658d4(0x42,1);
  }
  else {
    iVar2 = 0;
  }
  *param_4 = iVar2;
  if (iVar2 == 0) {
    iVar2 = FUN_1086b91c(4);
    if ((iVar2 == param_3) && (sVar1 = FUN_1086b91c(0x1e), sVar1 == 0xb)) {
      iVar2 = FUN_108658d4(0x42,1);
    }
    else {
      iVar2 = 0;
    }
    *param_4 = iVar2;
  }
  return;
}

