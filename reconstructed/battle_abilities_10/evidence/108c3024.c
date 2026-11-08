/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c3024(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  iVar3 = FUN_108544a0(param_2,param_3);
  if ((*(short *)(iVar3 + 0xc) == 0x1a5) && (iVar3 = FUN_1084be58(param_2), iVar3 == 1)) {
    uVar1 = FUN_1086b91c(3);
    iVar3 = FUN_1084c5a8(param_3,uVar1);
    if (iVar3 != 0) {
      uVar2 = FUN_1086b91c(0x12);
      iVar3 = thunk_FUN_0039ddc0(uVar2);
      if (iVar3 == 1) {
        FUN_1084ce84(0x36,0x1800);
        return;
      }
    }
  }
  return;
}

