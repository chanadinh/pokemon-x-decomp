/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b52bc(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 != param_3) {
    return;
  }
  uVar2 = FUN_1084c7bc(0x50);
  if (uVar2 < 0x28) {
    uVar3 = 0x28;
  }
  else if (uVar2 < 0x46) {
    uVar3 = 0x50;
  }
  else {
    uVar3 = 0x78;
  }
  FUN_108658d4(0x31,uVar3);
  return;
}

