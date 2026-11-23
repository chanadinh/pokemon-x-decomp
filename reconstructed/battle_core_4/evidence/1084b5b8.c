/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084b5b8(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 8);
  if (0x17 < param_2) {
    FUN_10800394(0,0);
  }
  iVar2 = iVar2 + param_2 * 4;
  if (*(int *)(iVar2 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar1 = *(int *)(iVar2 + 0x84);
  iVar2 = FUN_10874420(iVar1,0x11);
  if (((iVar2 != 0x67) && ((*(uint *)(iVar1 + 0x68) & 7) == 0)) &&
     (*(int *)(*(int *)(param_1 + 0x1c) + 0x290) == 0)) {
    return 1;
  }
  return 0;
}

