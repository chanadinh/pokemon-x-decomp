/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084c030(int param_1,uint param_2)

{
  char cVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 8);
  if (0x17 < param_2) {
    FUN_10800394(0,0);
  }
  iVar2 = iVar2 + param_2 * 4;
  if (*(int *)(iVar2 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  cVar1 = *(char *)(*(int *)(iVar2 + 0x84) + 0x171);
  if ((cVar1 == '\0') || (0x17 < (byte)(cVar1 - 1U))) {
    iVar2 = *(int *)(param_1 + 8);
    if (0x17 < param_2) {
      FUN_10800394(0,0);
    }
    iVar2 = iVar2 + param_2 * 4;
    if (*(int *)(iVar2 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    if ((*(uint *)(*(int *)(iVar2 + 0x84) + 0xa0) & 7) == 0) {
      return 0;
    }
  }
  return 1;
}

