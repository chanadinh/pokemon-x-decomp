/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_108ace94(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = &DAT_10b11c00 + param_1 * 0x44 + param_2 * 4;
  iVar1 = *piVar2;
  if (iVar1 != 0) {
    FUN_1084d1cc();
    *piVar2 = 0;
    piVar2[1] = piVar2[1] & 0xfffffff8;
    piVar2[3] = 0;
  }
  return iVar1 != 0;
}

