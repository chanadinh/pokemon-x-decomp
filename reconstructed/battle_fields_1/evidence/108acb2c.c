/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108acb2c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  
  if (*param_4 != 0) {
    iVar2 = FUN_1086b91c(0x1a);
    if ((iVar2 == 0) || (param_4[1] != iVar2)) {
      uVar1 = FUN_1086b91c(2);
      iVar3 = FUN_10869f7c(param_2,3,uVar1);
      FUN_1084d2d4(iVar3 + 4,2,0x34d);
      FUN_1084d29c(iVar3 + 4,uVar1);
      FUN_108802a8(iVar3 + 4);
      FUN_10867b48(param_2,iVar3);
      param_4[1] = iVar2;
    }
    *param_4 = 0;
  }
  return;
}

