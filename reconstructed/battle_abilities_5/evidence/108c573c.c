/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c573c(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar1 = FUN_1086b91c(2);
  iVar2 = FUN_1084c5a8(param_3,uVar1);
  if (iVar2 != 0) {
    uVar3 = FUN_108544a0(param_2,uVar1);
    iVar2 = FUN_1086f960(uVar3,0xb);
    if (iVar2 != 0) {
      FUN_108658d4(0x42,1);
      return;
    }
  }
  return;
}

