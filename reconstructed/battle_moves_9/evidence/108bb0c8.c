/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bb0c8(int param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    FUN_1084bef8(param_2,1);
    uVar1 = *(undefined2 *)(param_1 + 0x38);
    uVar3 = FUN_10873158(param_2,param_3);
    FUN_1081cf14(param_2,uVar3,uVar3,uVar1,0,0);
  }
  return;
}

