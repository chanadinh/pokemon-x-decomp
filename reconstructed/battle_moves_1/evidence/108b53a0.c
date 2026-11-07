/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b53a0(int param_1,int param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = *(undefined2 *)(param_1 + 0x38);
    uVar3 = FUN_1084d364(param_2);
    uVar4 = FUN_108c9ac8(param_2 + 0x20,uVar1,uVar3);
    if (1 < uVar4) {
      iVar2 = FUN_1086b91c(0x31);
      FUN_108658d4(0x31,iVar2 << 1);
      return;
    }
  }
  return;
}

