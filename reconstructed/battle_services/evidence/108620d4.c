/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_108620d4(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 0x210);
  if (iVar2 != 0) {
    if (param_2 == 9) {
      iVar1 = 9;
      if (*(int *)(iVar2 + 0x28c) != 0) {
        iVar1 = 0xb;
      }
      return iVar1;
    }
    if ((param_2 == 0xb) && (*(int *)(iVar2 + 0x28c) != 0)) {
      param_2 = 9;
    }
  }
  return param_2;
}

