/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc8dc(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  undefined4 uVar5;
  
  iVar2 = FUN_1086b91c(4);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x47), iVar2 == 0)) {
    uVar1 = FUN_1086b91c(3);
    uVar3 = FUN_108544a0(param_2,uVar1);
    iVar2 = FUN_1086f960(uVar3,0xb);
    if ((iVar2 == 0) &&
       ((iVar2 = FUN_10874420(uVar3,0x11), iVar2 != 0x8e &&
        (iVar2 = FUN_1084ae18(param_2,uVar1), iVar2 != 0x28a)))) {
      uVar4 = FUN_1084c7bc(0x1e);
      if (uVar4 < 0x15) {
        if (uVar4 < 0xb) {
          uVar3 = 2;
        }
        else {
          uVar3 = 1;
        }
      }
      else {
        uVar3 = 5;
      }
      uVar5 = FUN_10860e00(uVar3);
      FUN_1084ad50(param_2,param_3,uVar3,uVar5,0x1e);
    }
  }
  return;
}

