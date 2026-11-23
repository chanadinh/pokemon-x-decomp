/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084bac4(int param_1,uint param_2,uint param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  
  if (param_2 < 3) {
    iVar1 = *(byte *)(param_1 + 0x205) - param_2;
    if (iVar1 < 0) {
      iVar1 = iVar1 + 3;
    }
    uVar2 = (uint)*(byte *)(param_1 + iVar1 + 0x202);
    if (param_3 < uVar2) {
      FUN_108747fc(param_4,param_1 + iVar1 * 0x30 + ((uVar2 - param_3) - 1 & 0xff) * 8 + 0x172);
      return 1;
    }
  }
  return 0;
}

