/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8ec0(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (((iVar1 == param_3) && (iVar1 = FUN_108658d4(0x53,1), iVar1 != 0)) &&
     (iVar1 = FUN_1086b91c(2), iVar1 == param_3)) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084b804();
    if (iVar1 == 0) {
      iVar1 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar1 + 4,2,0x2fc);
      FUN_1084d29c(iVar1 + 4,param_3);
      uVar2 = FUN_1086b91c(0x12);
      FUN_1084d29c(iVar1 + 4,uVar2);
      FUN_10867b48(param_2,iVar1);
      return;
    }
  }
  return;
}

