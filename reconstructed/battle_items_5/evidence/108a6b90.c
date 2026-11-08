/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6b90(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (iVar2 = FUN_108544a0(param_2,param_3), *(short *)(iVar2 + 0xc) == 0x71)
     ) {
    cVar1 = FUN_1086b91c(0x2d);
    FUN_108658d4(0x2d,cVar1 + '\x02');
    return;
  }
  return;
}

