/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4918(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 != param_3) {
    return;
  }
  if (*(short *)(param_1 + 0x38) == 0xa3) {
    uVar2 = 0x1f9;
  }
  else if (*(short *)(param_1 + 0x38) == 0xa4) {
    uVar2 = 0x1f6;
  }
  else {
    uVar2 = 0x1ba;
  }
  FUN_1084c948(param_2,1,param_3);
  iVar1 = FUN_10869f7c(param_2,3,param_3);
  FUN_1084d2d4(iVar1 + 4,2,uVar2);
  FUN_1084d29c(iVar1 + 4,param_3);
  FUN_10867b48(param_2,iVar1);
  FUN_1084c948(param_2,2,param_3);
  return;
}

