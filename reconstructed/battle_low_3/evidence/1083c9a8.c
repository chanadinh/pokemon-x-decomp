/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1083c9a8(int *param_1,undefined2 param_2,char param_3,undefined4 param_4)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0xabe) {
    DAT_108fcb84 = 0xabe;
  }
  iVar1 = (**(code **)(*param_1 + 0xbc))(param_1);
  if (iVar1 == 0) {
    FUN_1089da54(*(undefined4 *)(param_1[100] + 0x7b0),param_2,(int)param_3);
    FUN_10858150(param_1[100] + 0x3a0,*(undefined4 *)(param_1[100] + 0x7b0),0,param_4,0,0);
  }
  return 1;
}

