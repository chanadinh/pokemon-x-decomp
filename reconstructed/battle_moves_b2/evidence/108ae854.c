/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ae854(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    iVar2 = FUN_10869f7c(param_2,0x34,param_3);
    uVar1 = FUN_1086b91c(6);
    *(undefined1 *)(iVar2 + 4) = uVar1;
    FUN_1084d2d4(iVar2 + 8,2,0x284);
    FUN_1084d29c(iVar2 + 8,param_3);
    FUN_1084d29c(iVar2 + 8,*(undefined1 *)(iVar2 + 4));
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

