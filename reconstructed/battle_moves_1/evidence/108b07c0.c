/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b07c0(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined2 uVar4;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 != param_3) {
    return;
  }
  uVar2 = FUN_108544a0(param_2,param_3);
  iVar1 = FUN_1084c224(uVar2,*(undefined2 *)(param_1 + 0x38));
  if (iVar1 == 4) {
    uVar4 = 0x28;
  }
  else {
    uVar3 = FUN_108520e4(uVar2,iVar1);
    uVar3 = uVar3 & 0xff;
    if (4 < uVar3) {
      uVar3 = 4;
    }
    uVar4 = *(undefined2 *)(&DAT_108d49ac + uVar3 * 2);
  }
  FUN_108658d4(0x31,uVar4);
  return;
}

