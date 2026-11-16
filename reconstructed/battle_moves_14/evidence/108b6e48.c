/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b6e48(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = FUN_1086b91c(2);
  bVar2 = iVar1 == param_3;
  if (bVar2) {
    iVar1 = *param_4;
  }
  if (!bVar2 || iVar1 != 2) {
    return;
  }
  FUN_1084d1cc(param_1);
  return;
}

