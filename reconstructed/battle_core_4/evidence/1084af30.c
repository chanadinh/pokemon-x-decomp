/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_1084af30(int param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 8);
  if (0x17 < param_2) {
    FUN_10800394(0,0);
  }
  iVar1 = iVar1 + param_2 * 4;
  if (*(int *)(iVar1 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar1 = FUN_10874348(*(undefined4 *)(iVar1 + 0x84));
  return iVar1 != 0x12;
}

