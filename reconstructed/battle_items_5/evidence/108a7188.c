/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7188(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if ((((iVar1 == param_3) &&
       (iVar1 = FUN_108544a0(param_2,param_3), *(short *)(iVar1 + 0xc) == 0x84)) &&
      (iVar1 = FUN_10851c28(iVar1), iVar1 == 0)) && (iVar1 = FUN_1086b91c(0x1b), iVar1 == 1)) {
    FUN_1084ce84(0x36,0x2000);
    return;
  }
  return;
}

