/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084c3cc(int *param_1,undefined4 param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  
  uVar1 = FUN_1087425c(param_2);
  uVar2 = param_1[1];
  bVar3 = **(byte **)(uVar2 + 0x10) < 2;
  if (bVar3) {
    uVar2 = (uint)*(byte *)(uVar2 + 0x815);
  }
  if (bVar3 && uVar2 == uVar1) {
    if (1 < param_3) {
      FUN_10800394(0,0);
      return;
    }
    *(undefined1 *)(*(int *)(*param_1 + 0x10) + param_3 + 0xa50) = 1;
  }
  return;
}

