/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108abb48(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x12), iVar1 != 0x1fb)) &&
     (iVar1 = FUN_108658d4(0x53,1), iVar1 != 0)) {
    *param_4 = 1;
  }
  return;
}

