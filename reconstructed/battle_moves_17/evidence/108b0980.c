/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b0980(int param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 local_10;
  
  local_10 = param_4;
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = *(undefined2 *)(param_1 + 0x38);
    local_10 = FUN_1086b91c(0x1f);
    FUN_1084b990(&local_10,uVar1);
    FUN_108658d4(0x1f,local_10);
  }
  return;
}

