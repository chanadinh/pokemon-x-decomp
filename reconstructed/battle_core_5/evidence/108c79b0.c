/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined2 FUN_108c79b0(int param_1,int param_2)

{
  undefined2 uVar1;
  
  param_1 = param_1 + param_2 * 0x38;
  if (*(int *)(param_1 + 0x838) == 0) {
    uVar1 = *(undefined2 *)(param_1 + 0x84c);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

