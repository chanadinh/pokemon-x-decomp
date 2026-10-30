/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1086b9a4(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  DAT_108fca68 = DAT_108fca68 + 1;
  FUN_10864e98(param_1,param_2,7,1);
  DAT_108fca68 = DAT_108fca68 + -1;
  iVar1 = DAT_108fca60;
  if (DAT_108fca68 == 0) {
    for (; iVar1 != 0; iVar1 = *(int *)(iVar1 + 4)) {
      *(undefined2 *)(iVar1 + 0x18) = 0;
    }
  }
  return;
}

