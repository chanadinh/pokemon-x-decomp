/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084a9d4(undefined4 param_1,int param_2,undefined4 param_3,int *param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_2) && (*param_4 != 0)) {
    iVar1 = FUN_1086b91c(0x1a);
    if ((iVar1 == 0) || (param_4[1] != iVar1)) {
      FUN_1084c948(param_1,1,param_3);
      iVar2 = FUN_10869f7c(param_1,3,param_3);
      FUN_1084d2d4(iVar2 + 4,2,param_5);
      FUN_1084d29c(iVar2 + 4,param_2);
      FUN_108802a8(iVar2 + 4);
      FUN_10867b48(param_1,iVar2);
      FUN_1084c948(param_1,2,param_3);
      param_4[1] = iVar1;
    }
    *param_4 = 0;
  }
  return;
}

