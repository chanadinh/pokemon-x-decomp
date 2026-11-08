/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7aa0(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x26), iVar2 == 0)) {
    uVar1 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    FUN_108658d4(0x27,uVar1);
    return;
  }
  return;
}

