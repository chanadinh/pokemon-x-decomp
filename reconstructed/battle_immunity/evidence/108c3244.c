/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c3244(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  iVar3 = FUN_1086b91c(4);
  if ((iVar3 == param_3) && (iVar3 = FUN_1086b91c(0x4f), iVar3 == 0)) {
    FUN_108544a0(param_2,param_3);
    iVar3 = FUN_1084b804();
    if (iVar3 == 0) {
      uVar2 = FUN_1086b91c(0x12);
      iVar3 = thunk_FUN_0039df44(uVar2,4);
      if ((iVar3 != 0) && (iVar3 = FUN_108658d4(0x41,1), iVar3 != 0)) {
        uVar1 = FUN_1086b91c(3);
        FUN_1084b3f8(param_2,param_3,uVar1);
        FUN_108658d4(0x53,1);
        return;
      }
    }
  }
  return;
}

