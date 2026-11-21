/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_108717dc(int param_1)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  if (*(char *)(*(int *)(param_1 + 0x10) + 0xb3) == '\0') {
    uVar1 = *(uint *)(param_1 + 0x804);
    uVar2 = uVar1;
    if ((*(byte *)(param_1 + 0x81c) & 8) == 0) {
      uVar3 = 1;
      if (*(char *)(param_1 + 0xa50) != '\0') {
        uVar3 = 2;
      }
      if (*(char *)(param_1 + 0xa51) != '\0') {
        uVar3 = uVar3 << 1;
      }
      if (1 < uVar3) {
        uVar2 = uVar1 * uVar3;
        if (99999 < uVar1 * uVar3) {
          uVar2 = 99999;
        }
      }
    }
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

