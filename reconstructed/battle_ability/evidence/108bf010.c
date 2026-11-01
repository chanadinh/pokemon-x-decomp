/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf010(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(4);
  if ((iVar2 == param_3) && (cVar1 = FUN_1086b91c(0x16), cVar1 == '\x0e' || cVar1 == '\t')) {
    FUN_1084ce84(0x36,0x800);
    return;
  }
  return;
}

