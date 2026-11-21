/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined2 FUN_10873524(int param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  
  if (((*(int *)(param_2 + 0x290) == 0) && ((*(uint *)(param_1 + 0x68) & 7) == 0)) &&
     (iVar2 = FUN_10874420(param_1,0x11), iVar2 != 0x67)) {
    uVar1 = *(undefined2 *)(param_1 + 0x12);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

