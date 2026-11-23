/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108c8510(int param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar1 = *(uint *)(param_1 + 0x30);
  if ((((uVar1 & 7) == 0) || ((uVar1 & 7) != 1)) || ((uVar1 & 0x1f8) != 0x78)) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

