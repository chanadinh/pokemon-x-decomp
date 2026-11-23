/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

byte FUN_108c82f4(int param_1)

{
  int iVar1;
  byte bVar2;
  
  bVar2 = *(char *)(param_1 + 0x815) + 2U & 3;
  iVar1 = FUN_10870368(param_1,bVar2);
  if (iVar1 == 0) {
    bVar2 = 4;
  }
  return bVar2;
}

