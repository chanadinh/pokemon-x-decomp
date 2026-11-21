/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10810cc0(int param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  byte *pbVar5;
  
  pbVar5 = (byte *)(param_1 + 0x18a6);
  if (5 < *pbVar5) {
    FUN_10800394(0,0);
  }
  bVar1 = *pbVar5;
  uVar3 = (uint)bVar1;
  if (uVar3 != 0) {
    uVar2 = uVar3 & 1;
    uVar4 = 0;
    if ((bVar1 & 1) != 0) {
      do {
        if (*(byte *)(param_1 + uVar4 + 0x189c) == param_2) {
          return;
        }
        uVar4 = uVar4 + 1;
      } while (uVar4 < uVar2);
    }
    for (; uVar2 < uVar3; uVar2 = uVar2 + 2) {
      if (*(byte *)(param_1 + uVar2 + 0x189c) == param_2) {
        return;
      }
      if (*(byte *)(param_1 + uVar2 + 0x189d) == param_2) {
        return;
      }
    }
  }
  *pbVar5 = bVar1 + 1;
  *(char *)(uVar3 + param_1 + 0x189c) = (char)param_2;
  return;
}

