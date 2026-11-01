/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8750(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    iVar2 = FUN_1084be58(param_2);
    uVar1 = FUN_1086b91c(0x16);
    uVar3 = iVar2 == 1;
    if ((bool)uVar3) {
      uVar1 = 9;
    }
    else if (iVar2 == 2) {
      uVar1 = 10;
      uVar3 = 4;
    }
    else if (iVar2 == 3) {
      uVar1 = 0xe;
      uVar3 = 2;
    }
    else if (iVar2 == 4) {
      uVar1 = 5;
      uVar3 = 3;
    }
    FUN_108658d4(0x16,uVar1);
    FUN_1084bef8(param_2,uVar3);
    return;
  }
  return;
}

