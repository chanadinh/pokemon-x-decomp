/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084b990(uint *param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = *param_1;
  uVar2 = uVar1 & 7;
  if (uVar2 == 1 || uVar2 == 3) {
    *param_1 = uVar1 & 0xfe0001ff | param_2 << 9;
    return;
  }
  if (uVar2 == 4) {
    uVar1 = uVar1 & 0x80007fff | param_2 << 0xf;
  }
  else {
    if (uVar2 != 2) {
      return;
    }
    uVar1 = uVar1 & 0xfe0001ff | param_2 << 9;
  }
  *param_1 = uVar1;
  return;
}

