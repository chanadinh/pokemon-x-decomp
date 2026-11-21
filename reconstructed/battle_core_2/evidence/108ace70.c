/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_108ace70(int param_1,int param_2)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = (&DAT_10b11c00)[param_1 * 0x44 + param_2 * 4];
  if (iVar1 != 0) {
    bVar2 = (*(uint *)(iVar1 + 0x18) & 0x2000000) == 0;
    if (bVar2) {
      *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) | 0x2000000;
    }
    return bVar2;
  }
  return false;
}

