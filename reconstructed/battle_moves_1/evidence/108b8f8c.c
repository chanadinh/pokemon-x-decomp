/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8f8c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    iVar1 = FUN_108544a0(param_2,param_3);
    uVar2 = FUN_1084cf74(*(undefined2 *)(iVar1 + 0x12),7);
    FUN_108658d4(0x31,uVar2);
    return;
  }
  return;
}

