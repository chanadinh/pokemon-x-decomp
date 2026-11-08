/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108abbec(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  iVar3 = FUN_1086b91c(4);
  if ((iVar3 == param_3) && (iVar3 = FUN_1086b91c(0x47), iVar3 == 0)) {
    uVar2 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar2,0);
    if (iVar3 != 0) {
      uVar1 = FUN_1086b91c(3);
      iVar3 = FUN_108544a0(param_2,uVar1);
      if (*(short *)(iVar3 + 0x12) == 0) {
        iVar3 = FUN_10869f7c(param_2,0x25,param_3);
        *(undefined1 *)(iVar3 + 4) = uVar1;
        FUN_10867b48(param_2,iVar3);
        return;
      }
    }
  }
  return;
}

