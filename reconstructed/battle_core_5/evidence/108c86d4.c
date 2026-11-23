/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_108c86d4(int param_1,uint param_2)

{
  int iVar1;
  
  if (param_2 < 3) {
    iVar1 = *(byte *)(param_1 + 0x205) - param_2;
    if (iVar1 < 0) {
      iVar1 = iVar1 + 3;
    }
    return *(undefined1 *)(param_1 + iVar1 + 0x202);
  }
  FUN_10800394(0,0);
  return 0;
}

