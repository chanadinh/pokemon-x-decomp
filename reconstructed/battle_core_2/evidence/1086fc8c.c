/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1086fc8c(int param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = (uint)*(byte *)(param_1 + 0x18);
  if (uVar3 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = uVar3 & 1;
    uVar1 = 0;
    if ((*(byte *)(param_1 + 0x18) & 1) != 0) {
      do {
        if (*(int *)(param_1 + uVar1 * 4) == param_2) {
          return uVar1;
        }
        uVar1 = uVar1 + 1;
      } while ((int)uVar1 < (int)uVar2);
    }
  }
  if (uVar2 < uVar3) {
    do {
      if (*(int *)(param_1 + uVar2 * 4) == param_2) {
        return uVar2;
      }
      if (*(int *)(param_1 + uVar2 * 4 + 4) == param_2) {
        return uVar2 + 1;
      }
      uVar2 = uVar2 + 2;
    } while ((int)uVar2 < (int)uVar3);
  }
  return 0xffffffff;
}

