/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4d08(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 != param_3) {
    return;
  }
  FUN_108544a0(param_2,param_3);
  uVar1 = FUN_1086b91c(4);
  iVar2 = FUN_108544a0(param_2,uVar1);
  uVar3 = FUN_1084b9f0(param_2,param_3);
  uVar4 = FUN_1084b9f0(param_2,*(undefined1 *)(iVar2 + 0x19));
  iVar2 = thunk_FUN_003006f8(uVar3,uVar4);
  if (iVar2 < 5) {
    if (iVar2 == 4) {
      uVar3 = 100;
    }
    else if (iVar2 == 3) {
      uVar3 = 0x50;
    }
    else if (iVar2 == 2) {
      uVar3 = 0x3c;
    }
    else {
      uVar3 = 0x28;
    }
  }
  else {
    uVar3 = 0x78;
  }
  FUN_108658d4(0x31,uVar3);
  return;
}

