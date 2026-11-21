/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10870434(int param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  if (*(byte *)(*(int *)(param_1 + 0xb8) + 0x18) < 2) {
    uVar1 = 0;
  }
  else {
    uVar1 = *(undefined4 *)(*(int *)(param_1 + 0xb8) + 4);
  }
  iVar2 = FUN_10874420(uVar1,0xd);
  if (iVar2 == 0) {
    if (*(byte *)(*(int *)(param_1 + 0xb8) + 0x18) < 3) {
      uVar1 = 0;
    }
    else {
      uVar1 = *(undefined4 *)(*(int *)(param_1 + 0xb8) + 8);
    }
    iVar2 = FUN_10874420(uVar1,0xd);
    uVar1 = 0;
    if (iVar2 != 0) {
      *param_2 = 0x26;
      return 4;
    }
  }
  else {
    *param_2 = 0x16;
    uVar1 = 4;
  }
  return uVar1;
}

