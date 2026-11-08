/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a69e4(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    iVar2 = FUN_108544a0(param_2,param_3);
    if (*(short *)(iVar2 + 0xc) == 0x1e7) {
      cVar1 = FUN_1086b91c(0x16);
      if ((cVar1 == '\x0f') || (cVar1 == '\a')) {
        FUN_1084ce84(0x32,0x1333);
        return;
      }
    }
  }
  return;
}

