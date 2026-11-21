/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b6ff8(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 != param_3) {
    return;
  }
  iVar1 = FUN_1084c0f8(0x14);
  if (iVar1 == 0) {
    iVar1 = *param_4;
  }
  else {
    iVar1 = FUN_108658d4(0x53,1);
    *param_4 = iVar1;
  }
  FUN_1084bef8(param_2,iVar1 != 0);
  return;
}

