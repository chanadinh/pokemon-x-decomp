/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c49c8(int param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (*param_4 == 0)) {
    FUN_1084bfc0(param_3);
    uVar2 = FUN_1084af00();
    FUN_1084c948(param_2,1,param_3);
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,1,0xb6);
    FUN_1084d29c(iVar1 + 4,uVar2);
    FUN_10867b48(param_2,iVar1);
    FUN_1084c948(param_2,2,param_3);
    *(code **)(param_1 + 0xc) = FUN_108c4e70;
    *param_4 = 1;
  }
  return;
}

