/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084bf24(int param_1,uint param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 8);
  if (0x17 < param_2) {
    FUN_10800394(0,0);
  }
  iVar2 = iVar2 + param_2 * 4;
  if (*(int *)(iVar2 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  if ((*(int *)(*(int *)(param_1 + 0x1c) + 0x27c) == 0) &&
     (iVar2 = FUN_108651d8(param_1,*(undefined4 *)(iVar2 + 0x84),1), iVar2 != 0)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

