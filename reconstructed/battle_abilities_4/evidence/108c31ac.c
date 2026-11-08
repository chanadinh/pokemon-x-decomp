/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c31ac(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = FUN_1086b91c(2);
  bVar2 = iVar1 == param_3;
  if (bVar2) {
    iVar1 = *param_4;
  }
  if ((bVar2 && iVar1 == 1) &&
     (iVar1 = FUN_108544a0(param_2,param_3), *(short *)(iVar1 + 0x12) == 0)) {
    FUN_1084ce84(0x36,0x2000);
    return;
  }
  return;
}

