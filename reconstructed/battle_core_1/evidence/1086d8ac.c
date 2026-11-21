/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1086d8ac(int param_1)

{
  undefined4 uVar1;
  
  if ((*(uint *)(param_1 + 0x20) & 7) == 0) {
    if ((*(uint *)(param_1 + 0x24) & 7) == 0) {
      if ((*(uint *)(param_1 + 0x28) & 7) == 0) {
        if ((*(uint *)(param_1 + 0x2c) & 7) == 0) {
          uVar1 = 0;
          if ((*(uint *)(param_1 + 0x30) & 7) != 0) {
            uVar1 = 5;
          }
        }
        else {
          uVar1 = 4;
        }
      }
      else {
        uVar1 = 3;
      }
    }
    else {
      uVar1 = 2;
    }
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

