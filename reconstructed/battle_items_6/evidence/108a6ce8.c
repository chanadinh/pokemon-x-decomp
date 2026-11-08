/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6ce8(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x3a), iVar1 == 3)) {
    uVar2 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    FUN_108658d4(0x25,uVar2);
    return;
  }
  return;
}

