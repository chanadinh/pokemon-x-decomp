/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a99a8(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  undefined2 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(4);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x47), iVar2 == 0)) {
    uVar1 = FUN_1086b91c(0x12);
    iVar2 = FUN_1084c808(uVar1);
    if ((iVar2 == 0) && (iVar2 = FUN_1086b91c(0x1b), iVar2 == 2)) {
      FUN_1084c96c(param_1);
      iVar2 = FUN_1086b91c(3);
      *param_4 = iVar2 + 1;
      iVar2 = FUN_10869f7c(param_2,0,param_3);
      *(uint *)(iVar2 + 4) = *(uint *)(iVar2 + 4) | 2;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

