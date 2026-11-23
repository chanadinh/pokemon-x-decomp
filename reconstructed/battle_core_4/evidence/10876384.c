/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10876384(int param_1,uint param_2)

{
  undefined4 uVar1;
  
  if (param_2 < *(byte *)(param_1 + 0x18)) {
    uVar1 = *(undefined4 *)(param_1 + param_2 * 4);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

