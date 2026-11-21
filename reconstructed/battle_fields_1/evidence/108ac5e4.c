/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ac5e4(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  
  uVar2 = FUN_1086b91c(4);
  uVar2 = uVar2 & 0xff;
  iVar3 = FUN_1084bfc0(uVar2);
  if ((iVar3 == param_3) && (uVar4 = FUN_1086b91c(3), uVar4 != uVar2)) {
    uVar1 = FUN_1086b91c(0x12);
    iVar3 = FUN_1086b91c(0x19);
    if ((0 < iVar3) &&
       ((iVar3 = thunk_FUN_0039df44(uVar1,3), iVar3 != 0 &&
        (iVar3 = FUN_108658d4(0x41,1), iVar3 != 0)))) {
      uVar5 = FUN_1086b91c(0x40);
      FUN_1084d2d4(uVar5,2,800);
      FUN_1084d29c(uVar5,uVar2);
      return;
    }
  }
  return;
}

