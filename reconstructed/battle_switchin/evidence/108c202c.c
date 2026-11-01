/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c202c(int param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  undefined4 uVar1;
  int iVar2;
  
  if (*param_4 == 0) {
    FUN_1084bfc0(param_3);
    uVar1 = FUN_1084af00();
    FUN_1084c948(param_2,1,param_3);
    iVar2 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar2 + 4,1,0xb6);
    FUN_1084d29c(iVar2 + 4,uVar1);
    FUN_10867b48(param_2,iVar2);
    FUN_1084c948(param_2,2,param_3);
    *(code **)(param_1 + 0xc) = FUN_108c4e70;
    *param_4 = 1;
  }
  return;
}

