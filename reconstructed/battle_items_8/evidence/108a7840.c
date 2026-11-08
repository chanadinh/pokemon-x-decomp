/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7840(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(2);
  if (iVar3 == param_3) {
    uVar4 = FUN_108544a0(param_2,param_3);
    uVar1 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    iVar3 = FUN_10869f7c(param_2,4,param_3);
    *(char *)(iVar3 + 6) = (char)param_3;
    uVar2 = FUN_1084d2f4(uVar4,uVar1);
    *(undefined2 *)(iVar3 + 4) = uVar2;
    FUN_10867b48(param_2,iVar3);
    return;
  }
  return;
}

