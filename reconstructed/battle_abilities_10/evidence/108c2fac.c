/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c2fac(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_108544a0(param_2,param_3);
  if ((*(short *)(iVar2 + 0xc) == 0x1a5) && (iVar2 = FUN_1084be58(param_2), iVar2 == 1)) {
    uVar1 = FUN_1086b91c(4);
    iVar2 = FUN_1084c5a8(param_3,uVar1);
    if ((iVar2 != 0) && (iVar2 = FUN_1086b91c(0x1b), iVar2 == 2)) {
      FUN_1084ce84(0x36,0x1800);
      return;
    }
  }
  return;
}

