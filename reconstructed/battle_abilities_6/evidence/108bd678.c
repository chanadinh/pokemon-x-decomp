/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd678(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (cVar1 = FUN_1086b91c(0x26), cVar1 == '\0')) {
    FUN_108658d4(0x27,10);
    return;
  }
  return;
}

