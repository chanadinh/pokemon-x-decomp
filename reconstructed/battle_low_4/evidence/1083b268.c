/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083b268(int *param_1,undefined4 param_2)

{
  undefined1 uVar1;
  int iVar2;
  
  if (DAT_108fcb84 != 0xbf7) {
    DAT_108fcb84 = 0xbf7;
  }
  param_1[0x1ad] = 0;
  param_1[0x1ae] = 0;
  FUN_1086fd18((undefined1 *)((int)param_1 + 0x6ca),0xff);
  *(undefined1 *)(param_1 + 0x1b2) = 0xff;
  *(undefined1 *)((int)param_1 + 0x6c9) = 0xff;
  *(undefined1 *)((int)param_1 + 0x25d) = 0;
  uVar1 = (**(code **)(*param_1 + 0x10c))(param_1,param_2);
  *(undefined1 *)(param_1 + 0x1b2) = uVar1;
  uVar1 = (**(code **)(*param_1 + 0x10c))(param_1,param_2);
  *(undefined1 *)((int)param_1 + 0x6ca) = uVar1;
  iVar2 = (**(code **)(*param_1 + 0xbc))(param_1);
  if (iVar2 != 0) {
    param_1[0x1ae] = 0xff;
  }
  return;
}

