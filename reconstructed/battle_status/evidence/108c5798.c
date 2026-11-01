/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c5798(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = FUN_1086b91c(4);
  iVar2 = FUN_1084c5a8(param_3,uVar1);
  iVar3 = 0;
  if (iVar2 != 0) {
    iVar3 = *param_4;
  }
  if (iVar2 != 0 && iVar3 != 0) {
    FUN_1084c948(param_2,1,param_3);
    iVar3 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar3 + 4,2,0x4a2);
    FUN_1084d29c(iVar3 + 4,uVar1);
    FUN_10867b48(param_2,iVar3);
    FUN_1084c948(param_2,2,param_3);
    *param_4 = 0;
  }
  return;
}

