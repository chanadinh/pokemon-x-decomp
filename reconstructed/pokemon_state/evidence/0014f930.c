/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0014f930(int param_1,undefined2 param_2)

{
  int iVar1;
  
  if (*(int *)(param_1 + 4) != 0) {
    FUN_00168660(param_1);
    iVar1 = DAT_0014f97c;
    if ((*(ushort *)(*(int *)(param_1 + 8) + 4) & 4) == 0) {
      iVar1 = *(int *)(param_1 + 4);
    }
    *(undefined2 *)(iVar1 + 8) = param_2;
    FUN_001685f0(param_1);
    return;
  }
  return;
}

