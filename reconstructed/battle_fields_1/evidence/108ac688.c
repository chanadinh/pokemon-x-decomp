/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ac688(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar1 = FUN_1086b91c(4);
  iVar2 = FUN_1084bfc0(uVar1);
  if (((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x1b), iVar2 == 1)) &&
     (iVar2 = FUN_1086b91c(0x46), iVar2 == 0)) {
    iVar2 = FUN_1084bf14(param_2);
    if ((iVar2 == 0) || (iVar2 == 3)) {
      uVar3 = 0x800;
    }
    else {
      uVar3 = 0xaac;
    }
    FUN_1084ce84(0x36,uVar3);
    return;
  }
  return;
}

