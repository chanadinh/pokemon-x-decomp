/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b164c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 != param_3) {
    return;
  }
  uVar3 = FUN_108544a0(param_2,param_3);
  uVar1 = FUN_1086b91c(4);
  uVar4 = FUN_108544a0(param_2,uVar1);
  uVar3 = FUN_108676cc(param_2,uVar3,0);
  uVar4 = FUN_108676cc(param_2,uVar4,0);
  iVar2 = thunk_FUN_003006f8(uVar3,uVar4);
  if (iVar2 < 4) {
    if (iVar2 < 3) {
      if (iVar2 < 2) {
        if (iVar2 < 1) {
          uVar3 = 0x28;
        }
        else {
          uVar3 = 0x3c;
        }
      }
      else {
        uVar3 = 0x50;
      }
    }
    else {
      uVar3 = 0x78;
    }
  }
  else {
    uVar3 = 0x96;
  }
  FUN_108658d4(0x31,uVar3);
  return;
}

