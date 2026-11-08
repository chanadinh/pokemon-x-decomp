/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108afbcc(int param_1,int param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    uVar1 = *(undefined2 *)(param_1 + 0x38);
    uVar3 = FUN_1084d364(param_2);
    iVar2 = FUN_108c9ac8(param_2 + 0x20,uVar1,uVar3);
    if (iVar2 == 1) {
      iVar2 = FUN_10869f7c(param_2,0x31,param_3);
      *(undefined2 *)(iVar2 + 4) = uVar1;
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

