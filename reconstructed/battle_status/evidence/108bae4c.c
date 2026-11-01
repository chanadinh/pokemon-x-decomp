/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bae4c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  undefined1 uVar1;
  int iVar2;
  
  if (((*(int *)(param_4 + 0x18) != 0) && (iVar2 = FUN_1086b91c(0x1e), iVar2 == 2)) &&
     (iVar2 = FUN_108658d4(0x42,1), iVar2 != 0)) {
    iVar2 = FUN_10869f7c(param_2,3,param_3);
    uVar1 = FUN_1086b91c(4);
    FUN_1084d2d4(iVar2 + 4,2);
    FUN_1084d29c(iVar2 + 4,uVar1);
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

