/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bb270(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  iVar3 = FUN_1084bf14(param_2);
  if ((iVar3 != 0) && (iVar3 = FUN_1084bf14(param_2), iVar3 != 3)) {
    uVar1 = FUN_1086b91c(3);
    uVar4 = FUN_108544a0(param_2,param_3);
    uVar5 = FUN_108544a0(param_2,uVar1);
    iVar3 = FUN_1084c5a8(param_3,uVar1);
    if ((iVar3 == 0) &&
       (((iVar3 = FUN_1086b91c(0x12), iVar3 != 0x1fb &&
         (iVar3 = FUN_10856a88(uVar4,0x21), iVar3 == 0)) &&
        (iVar3 = FUN_1084c020(param_2), iVar3 == 0)))) {
      if (*(short *)(param_1 + 0x38) == 0x1dc) {
        iVar3 = FUN_1086f960(uVar5,0xb);
        if (iVar3 != 0) {
          return;
        }
        iVar3 = FUN_10874420(uVar5,0x11);
        if (iVar3 == 0x8e) {
          return;
        }
        iVar3 = FUN_1084ae18(param_2,uVar1);
        if (iVar3 == 0x28a) {
          return;
        }
      }
      uVar2 = FUN_1086b91c(0x12);
      iVar3 = FUN_1084b67c(param_2,uVar1,param_3,uVar2);
      if (iVar3 == 0) {
        FUN_108658d4(4,param_3);
        return;
      }
    }
  }
  return;
}

