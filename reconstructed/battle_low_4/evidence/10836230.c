/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10836230(int param_1)

{
  if (*(int **)(param_1 + 0x70) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x70) + 4))();
    *(undefined4 *)(param_1 + 0x70) = 0;
  }
  if (*(int **)(param_1 + 0x6c) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x6c) + 4))();
    *(undefined4 *)(param_1 + 0x6c) = 0;
  }
  if (*(int **)(param_1 + 0x68) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x68) + 4))();
    *(undefined4 *)(param_1 + 0x68) = 0;
  }
  if (*(int **)(param_1 + 100) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 100) + 4))();
    *(undefined4 *)(param_1 + 100) = 0;
  }
  if (*(int **)(param_1 + 0x60) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x60) + 4))();
    *(undefined4 *)(param_1 + 0x60) = 0;
  }
  if (*(int **)(param_1 + 0x5c) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x5c) + 4))();
    *(undefined4 *)(param_1 + 0x5c) = 0;
  }
  if (*(int **)(param_1 + 0x58) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x58) + 4))();
    *(undefined4 *)(param_1 + 0x58) = 0;
  }
  if (*(int **)(param_1 + 0x54) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x54) + 4))();
    *(undefined4 *)(param_1 + 0x54) = 0;
  }
  if (*(int **)(param_1 + 0x50) != (int *)0x0) {
    (**(code **)(**(int **)(param_1 + 0x50) + 4))();
    *(undefined4 *)(param_1 + 0x50) = 0;
  }
  return;
}

