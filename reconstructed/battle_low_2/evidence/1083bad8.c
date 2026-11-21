/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083bad8(int param_1)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x1155) {
    DAT_108fcb84 = 0x1155;
  }
  iVar1 = *(int *)(param_1 + 400);
  thunk_FUN_0014ddf4(*(undefined4 *)(iVar1 + 0x65c),3);
  thunk_FUN_0011d40c(*(undefined4 *)(iVar1 + 0x65c),2,1);
  return;
}

