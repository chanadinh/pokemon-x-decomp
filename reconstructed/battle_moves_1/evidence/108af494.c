/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108af494(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_108544a0(param_2,param_3), *(short *)(iVar1 + 0x12) == 0))
  {
    iVar1 = FUN_1086b91c(0x31);
    FUN_108658d4(0x31,iVar1 << 1);
    return;
  }
  return;
}

