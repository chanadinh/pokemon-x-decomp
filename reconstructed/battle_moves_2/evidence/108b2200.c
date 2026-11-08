/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b2200(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  if (((*param_4 == 2) && (iVar1 = FUN_1086b91c(3), iVar1 == param_3)) &&
     (iVar1 = FUN_1086b91c(0x16), iVar1 == 0xc)) {
    FUN_1084ce84(0x32,0x2000);
    return;
  }
  return;
}

