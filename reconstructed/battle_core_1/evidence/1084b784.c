/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084b784(int param_1,uint param_2,undefined4 *param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  uVar1 = 0;
  if (*(byte *)(param_1 + 0x108c) != 0) {
    do {
      iVar2 = param_1 + uVar1 * 0x10;
      if (((*(byte *)(*(int *)(iVar2 + 0x10bc) + 0x19) == param_2) &&
          (uVar3 = *(uint *)(iVar2 + 0x10c0) & 0xf, uVar3 != 6)) && (uVar3 != 8)) {
        *param_3 = *(undefined4 *)(param_1 + uVar1 * 0x10 + 0x10c0);
        return 1;
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < *(byte *)(param_1 + 0x108c));
  }
  return 0;
}

