/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a64bc(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_108544a0(param_2,param_3), *(short *)(iVar1 + 0xc) == 0x19)
     ) {
    FUN_1084ce84(0x36,0x2000);
    return;
  }
  return;
}

