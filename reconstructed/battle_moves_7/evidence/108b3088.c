/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b3088(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 != param_3) {
    return;
  }
  iVar3 = FUN_10869f7c(param_2,0x2f,param_3);
  uVar1 = FUN_1086b91c(6);
  *(undefined1 *)(iVar3 + 6) = uVar1;
  if (*(short *)(param_1 + 0x38) == 0x1fd) {
    uVar2 = 0x2c7;
  }
  else {
    uVar2 = 0x2c8;
  }
  *(undefined2 *)(iVar3 + 4) = uVar2;
  FUN_10867b48(param_2,iVar3);
  return;
}

