/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_108c87a0(int param_1,uint param_2)

{
  undefined1 uVar1;
  
  if (param_2 < *(byte *)(param_1 + 0x19)) {
    uVar1 = *(undefined1 *)(param_1 + param_2);
  }
  else {
    uVar1 = 6;
  }
  return uVar1;
}

