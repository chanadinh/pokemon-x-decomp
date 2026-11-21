/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1081d748(int param_1,undefined1 param_2)

{
  byte bVar1;
  
  bVar1 = *(byte *)(param_1 + 0x1091);
  if (bVar1 < 10) {
    *(byte *)(param_1 + 0x1091) = bVar1 + 1;
    *(undefined1 *)((uint)bVar1 + param_1 + 0x1094) = param_2;
  }
  return;
}

