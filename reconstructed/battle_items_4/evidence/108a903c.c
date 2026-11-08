/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a903c(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x16), iVar1 == 4)) &&
     ((*param_4 == 1 || (iVar1 = FUN_1084c70c(param_2), iVar1 != 0)))) {
    FUN_1084ce84(0x32,0x14cd);
    return;
  }
  return;
}

