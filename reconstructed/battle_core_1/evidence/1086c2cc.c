/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1086c2cc(int param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  while( true ) {
    iVar1 = uVar2 * 3;
    if ((*(char *)(param_1 + iVar1) != '\0') && (*(byte *)(iVar1 + param_1 + 2) == param_2)) {
      return uVar2 & 0xff;
    }
    if ((*(char *)(iVar1 + param_1 + 3) != '\0') && (*(byte *)(iVar1 + param_1 + 5) == param_2))
    break;
    uVar2 = uVar2 + 2;
    if (9 < uVar2) {
      return 10;
    }
  }
  return uVar2 + 1 & 0xff;
}

