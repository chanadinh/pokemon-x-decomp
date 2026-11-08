/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6564(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) &&
     (iVar2 = FUN_108544a0(param_2,param_3),
     *(short *)(iVar2 + 0xc) == 0x68 || *(short *)(iVar2 + 0xc) == 0x69)) {
    uVar1 = FUN_1086b91c(0x12);
    iVar2 = thunk_FUN_0039ddc0(uVar1);
    if (iVar2 == 1) {
      FUN_1084ce84(0x36,0x2000);
      return;
    }
  }
  return;
}

