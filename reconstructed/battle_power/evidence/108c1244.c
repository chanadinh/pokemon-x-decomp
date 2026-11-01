/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c1244(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (((iVar2 == param_3) && (iVar2 = FUN_1084be58(param_2), iVar2 == 4)) &&
     ((cVar1 = FUN_1086b91c(0x16), cVar1 == '\x04' || (cVar1 == '\x05' || cVar1 == '\b')))) {
    FUN_1084ce84(0x32,0x14cd);
    return;
  }
  return;
}

