/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c53cc(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_10865824(param_2,param_3,9);
  if (iVar1 != 0) {
    FUN_1084c948(param_2,1,param_3);
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084ca4c(uVar2,0xd);
    if (iVar1 == 0) {
      iVar1 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar1 + 4,2,0x1ab);
      FUN_1084d29c(iVar1 + 4,param_3);
      FUN_10867b48(param_2,iVar1);
      iVar1 = FUN_10869f7c(param_2,0x17,param_3);
      *(char *)(iVar1 + 5) = (char)param_3;
      *(undefined1 *)(iVar1 + 4) = 0xd;
      FUN_10867b48(param_2);
    }
    else {
      iVar1 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar1 + 4,2,0xd2);
      FUN_1084d29c(iVar1 + 4,param_3);
      FUN_10867b48(param_2,iVar1);
    }
    FUN_1084c948(param_2,2,param_3);
    return;
  }
  return;
}

