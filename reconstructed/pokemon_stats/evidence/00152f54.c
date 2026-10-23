/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined2 FUN_00152f54(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  
  uVar1 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    FUN_00168660(param_1);
    iVar2 = DAT_00152f9c;
    if ((*(ushort *)(*(int *)(param_1 + 8) + 4) & 4) == 0) {
      iVar2 = *(int *)(param_1 + 4);
    }
    uVar1 = *(undefined2 *)(iVar2 + 10);
    FUN_001685f0(param_1);
  }
  return uVar1;
}

