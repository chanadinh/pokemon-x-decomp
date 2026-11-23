/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083b3cc(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0xa7f) {
    DAT_108fcb84 = 0xa7f;
  }
  iVar1 = (**(code **)(*param_1 + 0xbc))(param_1);
  if (iVar1 == 0) {
    FUN_1089d648(param_1[0x78],param_2,param_3);
    FUN_10858150(param_1[100] + 0x3a0,param_1[0x78],0,0,0,0);
  }
  return;
}

