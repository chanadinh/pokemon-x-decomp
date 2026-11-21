/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108afa98(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1086d8ac();
    if ((iVar1 == 5 || iVar1 == 1) || iVar1 == 4) {
      iVar2 = FUN_10869f7c(param_2,10,param_3);
      *(char *)(iVar2 + 4) = (char)iVar1;
      *(undefined1 *)(iVar2 + 0x11) = 1;
      *(char *)(iVar2 + 5) = (char)param_3;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

