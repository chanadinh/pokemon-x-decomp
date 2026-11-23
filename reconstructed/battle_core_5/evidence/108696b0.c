/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108696b0(int param_1,int param_2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar3 = (uint)*(byte *)(param_1 + 0x6e);
  if (9 < uVar3) {
    FUN_10800394(0,0);
    return;
  }
  if (uVar3 != 0) {
    uVar2 = uVar3 & 1;
    uVar4 = 0;
    if ((*(byte *)(param_1 + 0x6e) & 1) != 0) {
      do {
        if (*(int *)(param_1 + uVar4 * 4) == param_2) {
          return;
        }
        uVar4 = uVar4 + 1;
      } while (uVar4 < uVar2);
    }
    for (; uVar2 < uVar3; uVar2 = uVar2 + 2) {
      if (*(int *)(param_1 + uVar2 * 4) == param_2) {
        return;
      }
      if (*(int *)(param_1 + uVar2 * 4 + 4) == param_2) {
        return;
      }
    }
  }
  *(int *)(param_1 + uVar3 * 4) = param_2;
  *(undefined1 *)((uint)*(byte *)(param_1 + 0x6e) + param_1 + 0x50) = 0;
  *(undefined2 *)(param_1 + (uint)*(byte *)(param_1 + 0x6e) * 2 + 0x28) = 0;
  *(undefined2 *)(param_1 + (uint)*(byte *)(param_1 + 0x6e) * 2 + 0x3c) = 0;
  bVar1 = *(char *)(param_1 + 0x6e) + 1;
  *(byte *)(param_1 + 0x6e) = bVar1;
  if (*(byte *)(param_1 + 0x6f) < bVar1) {
    *(byte *)(param_1 + 0x6f) = bVar1;
  }
  return;
}

