/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1087198c(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  if ((((param_1 == 0) || (iVar1 = FUN_10874420(param_1,0xd), iVar1 == 0)) ||
      ((*(uint *)(param_1 + 0x24) & 7) != 0)) || ((*(uint *)(param_1 + 0x28) & 7) != 0)) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

