/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1084d2f4(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = FUN_10874420(param_1,0xe);
  iVar2 = thunk_FUN_0015e958(uVar1,param_2);
  if (iVar2 == 0) {
    iVar2 = 1;
  }
  return iVar2;
}

