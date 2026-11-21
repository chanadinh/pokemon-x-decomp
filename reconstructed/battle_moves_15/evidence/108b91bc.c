/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b91bc(undefined4 param_1,undefined4 param_2,uint param_3,undefined4 *param_4)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  
  uVar2 = FUN_1086b91c(3);
  if ((uVar2 & 0xff) != param_3) {
    uVar1 = FUN_1086b91c(0x12);
    iVar3 = thunk_FUN_0039df44(uVar1,5);
    if (iVar3 != 0) {
      uVar4 = FUN_108544a0(param_2,param_3);
      iVar3 = FUN_10856a88(uVar4,0x21);
      if ((iVar3 == 0) && (iVar3 = FUN_108658d4(2,param_3), iVar3 != 0)) {
        FUN_108658d4(4,param_3);
        *param_4 = 1;
      }
    }
  }
  return;
}

