/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081ca98(int *param_1,int param_2)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  
  if (**(byte **)(param_1[1] + 0x10) < 2) {
    iVar2 = *(int *)(*param_1 + 0x10);
    if ((*(byte *)(iVar2 + 0x81c) & 8) == 0) {
      uVar3 = param_2 + *(int *)(iVar2 + 0x804);
      *(uint *)(iVar2 + 0x804) = uVar3;
      if (99999 < uVar3) {
        uVar3 = 99999;
      }
      *(uint *)(iVar2 + 0x804) = uVar3;
    }
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

