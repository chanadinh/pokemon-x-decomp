/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a98f8(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 local_10;
  
  local_10 = param_4;
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x1e), iVar1 == 8)) {
    local_10 = FUN_1086b91c(0x1f);
    FUN_10876cf4(&local_10,8);
    FUN_108658d4(0x1f,local_10);
  }
  return;
}

