/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10876ca8(uint *param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = *param_1;
  uVar2 = uVar1 & 7;
  if (uVar2 == 1 || uVar2 == 3) {
    *param_1 = uVar1 & 0xfdffffff | param_2 << 0x19;
    return;
  }
  if (uVar2 == 4) {
    uVar1 = uVar1 & 0x7fffffff | param_2 << 0x1f;
  }
  else {
    if (uVar2 != 2) {
      return;
    }
    uVar1 = uVar1 & 0xfdffffff | param_2 << 0x19;
  }
  *param_1 = uVar1;
  return;
}

