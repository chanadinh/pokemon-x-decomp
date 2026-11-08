/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6a64(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    iVar1 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar1 + 4) = 5;
    *(undefined1 *)(iVar1 + 0x17) = 1;
    *(char *)(iVar1 + 0xc) = (char)param_3;
    *(undefined1 *)(iVar1 + 0x16) = 1;
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

