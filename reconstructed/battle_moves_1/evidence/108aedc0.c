/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aedc0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 != param_3) {
    return;
  }
  uVar1 = FUN_1086b91c(4);
  uVar3 = FUN_108544a0(param_2,uVar1);
  iVar2 = 0;
  uVar5 = 0;
  do {
    iVar4 = FUN_10874420(uVar3,(&DAT_108d2c3f)[uVar5]);
    if (6 < iVar4) {
      iVar2 = iVar4 + iVar2 + -6;
    }
    uVar5 = uVar5 + 1;
  } while (uVar5 < 7);
  uVar5 = iVar2 * 0x14 + 0x3c;
  if (200 < uVar5) {
    uVar5 = 200;
  }
  FUN_108658d4(0x31,uVar5);
  return;
}

