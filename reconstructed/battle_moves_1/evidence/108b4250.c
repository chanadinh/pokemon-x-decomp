/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4250(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(4);
    FUN_108544a0(param_2,uVar1);
    uVar3 = FUN_108713a0();
    uVar4 = FUN_1089e5f8(0x78,uVar3);
    uVar4 = uVar4 / 100;
    if (uVar4 == 0) {
      uVar4 = 1;
    }
    FUN_108658d4(0x31,uVar4);
    return;
  }
  return;
}

