/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168660(int param_1)

{
  ushort uVar1;
  uint uVar2;
  
  if (*(char *)(param_1 + 0xc) != '\0') {
    FUN_00174e6c(*(undefined4 **)(param_1 + 8) + 2,0xe0,**(undefined4 **)(param_1 + 8));
    if (*(int *)(param_1 + 4) != 0) {
      FUN_00174e6c(*(int *)(param_1 + 4),0x1c,**(undefined4 **)(param_1 + 8));
    }
    *(undefined1 *)(param_1 + 0xc) = 0;
    uVar1 = *(ushort *)(*(int *)(param_1 + 8) + 6);
    uVar2 = FUN_00174e0c(*(int *)(param_1 + 8) + 8,0xe0);
    if (uVar1 != uVar2) {
      *(ushort *)(*(int *)(param_1 + 8) + 4) = *(ushort *)(*(int *)(param_1 + 8) + 4) | 4;
    }
  }
  return;
}

