/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083b530(int param_1)

{
  int *piVar1;
  int iVar2;
  
  if (DAT_108fcb84 != 0x1465) {
    DAT_108fcb84 = 0x1465;
  }
  iVar2 = *(int *)(param_1 + 400);
  piVar1 = *(int **)(iVar2 + 0x240);
  if (piVar1 != (int *)0x0) {
    (**(code **)(*piVar1 + 4))();
    *(undefined4 *)(iVar2 + 0x240) = 0;
  }
  return;
}

