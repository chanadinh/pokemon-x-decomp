/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b26c4(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = FUN_1086b91c(3);
  bVar2 = iVar1 == param_3;
  if (bVar2) {
    iVar1 = *(int *)(param_4 + 0x18);
  }
  if (bVar2 && iVar1 == 0) {
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,2,0x2f9);
    FUN_1084d29c(iVar1 + 4,param_3);
    FUN_10867b48(param_2,iVar1);
    *(undefined4 *)(param_4 + 0x18) = 1;
  }
  return;
}

