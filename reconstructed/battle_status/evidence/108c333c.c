/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c333c(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  uVar1 = FUN_1086b91c(4);
  uVar2 = FUN_1086b91c(0x1e);
  iVar3 = FUN_1084c5a8(param_3,uVar1);
  if ((iVar3 != 0) && (iVar3 = FUN_108a4764(uVar2), iVar3 != 0)) {
    iVar3 = FUN_108658d4(0x42,1);
    *param_4 = iVar3;
    if (iVar3 != 0) {
      FUN_108658d4(0x44,1);
      return;
    }
  }
  return;
}

