/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084c284(int param_1,undefined4 param_2,int param_3,undefined4 *param_4,int param_5)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x16), iVar2 == param_5)) {
    uVar1 = FUN_1086b91c(0x12);
    iVar2 = thunk_FUN_0039e058(uVar1);
    if ((iVar2 != 0) &&
       (((iVar2 = FUN_1084c720(uVar1), iVar2 == 0 && (iVar2 = FUN_10881638(param_1), iVar2 == 0)) &&
        (iVar2 = FUN_1084c70c(param_2), iVar2 == 0)))) {
      FUN_1084c700(param_1);
      iVar2 = FUN_10869f7c(param_2,0x24,param_3);
      FUN_1084d2d4(iVar2 + 8,1,0xbc);
      FUN_1084d29c(iVar2 + 8,*(undefined2 *)(param_1 + 0x38));
      uVar3 = FUN_1086b91c(0x12);
      FUN_1084d29c(iVar2 + 8,uVar3);
      FUN_10867b48(param_2,iVar2);
      *param_4 = 1;
    }
  }
  return;
}

