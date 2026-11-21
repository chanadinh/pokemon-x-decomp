/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7670(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 != param_3) {
    return;
  }
  uVar2 = FUN_108544a0(param_2,param_3);
  iVar1 = FUN_1084bb2c(uVar2,0);
  if (iVar1 == 2) {
    uVar2 = 0x800;
  }
  else if (iVar1 == 3) {
    uVar2 = 0x1000;
  }
  else {
    uVar2 = 0x400;
  }
  FUN_108658d4(0x36,uVar2);
  return;
}

