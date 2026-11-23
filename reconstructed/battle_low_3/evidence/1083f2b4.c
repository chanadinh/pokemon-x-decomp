/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083f2b4(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  if (DAT_108fcb84 != 0x111c) {
    DAT_108fcb84 = 0x111c;
  }
  FUN_1086fd18(param_1 + 0x6ca,0xff);
  *(undefined1 *)(param_1 + 0x6c9) = 0xff;
  *(undefined1 *)(param_1 + 0x25d) = 0;
  *(char *)(param_1 + 0x6c8) = (char)param_2;
  iVar1 = (**(code **)(**(int **)(param_1 + param_2 * 4 + 0x100) + 0x7c))();
  if ((iVar1 == 0) &&
     (*(char *)(*(int *)(param_1 + (uint)*(byte *)(param_1 + 0x6c8) * 4 + 0x100) + 0x15c) == '\0'))
  {
    uVar2 = 100;
  }
  else {
    uVar2 = 1;
    *(undefined4 *)(param_1 + 0x6b4) = 0;
  }
  *(undefined4 *)(param_1 + 0x6b8) = uVar2;
  return;
}

