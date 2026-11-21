/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1081e28c(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = param_2 + 2U & 3;
  iVar1 = FUN_10870368(*(undefined4 *)(param_1 + 4),uVar2);
  if (iVar1 == 0) {
    uVar2 = 4;
  }
  return uVar2;
}

