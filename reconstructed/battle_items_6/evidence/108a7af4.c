/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a7af4(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 local_10;
  
  local_10 = param_4;
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    local_10 = FUN_1086b91c(0x1f);
    FUN_10876ca8(&local_10,1);
    FUN_108658d4(0x1f,local_10);
  }
  return;
}

