/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10867a04(int param_1,undefined4 *param_2,ushort param_3,int param_4)

{
  int iVar1;
  
  iVar1 = thunk_FUN_001ce05c(*(undefined4 *)(*(int *)(param_1 + 4) + 0x10),4);
  if (iVar1 != 0) {
    return;
  }
  if (param_3 < *(ushort *)(param_2 + 4)) {
    *(ushort *)(param_2 + 4) = *(ushort *)(param_2 + 4) - param_3;
  }
  else {
    *(undefined2 *)(param_2 + 4) = 0;
    thunk_FUN_0039cbe0(*param_2);
  }
  if (param_4 == 0) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),1,*(undefined1 *)((int)param_2 + 0x19),param_3);
    return;
  }
  FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),2,*(undefined1 *)((int)param_2 + 0x19),param_3);
  return;
}

