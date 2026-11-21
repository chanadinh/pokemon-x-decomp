/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108acc24(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  undefined1 uVar1;
  int iVar2;
  
  if (*param_4 != 0) {
    uVar1 = FUN_1086b91c(4);
    iVar2 = FUN_10869f7c(param_2,3,uVar1);
    FUN_1084d2d4(iVar2 + 4,2,0x34a);
    FUN_1084d29c(iVar2 + 4,uVar1);
    FUN_10867b48(param_2,iVar2);
    *param_4 = 0;
  }
  return;
}

