/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083b9f0(int *param_1,undefined4 param_2)

{
  byte bVar1;
  undefined1 uVar2;
  int iVar3;
  
  if (DAT_108fcb84 != 0x116b) {
    DAT_108fcb84 = 0x116b;
  }
  FUN_1086fd18((int)param_1 + 0x6ca,0xff);
  *(undefined1 *)(param_1 + 0x1b2) = 0xff;
  *(undefined1 *)((int)param_1 + 0x6c9) = 0xff;
  *(undefined1 *)((int)param_1 + 0x25d) = 0;
  bVar1 = (**(code **)(*param_1 + 0x10c))(param_1,param_2);
  *(byte *)(param_1 + 0x1b2) = bVar1;
  uVar2 = (**(code **)(*param_1 + 0x10c))(param_1,param_2);
  *(undefined1 *)((int)param_1 + 0x6c9) = uVar2;
  iVar3 = (**(code **)(*(int *)param_1[*(byte *)(param_1 + 0x1b2) + 0x40] + 0x7c))();
  if ((iVar3 == 0) && (*(char *)(param_1[*(byte *)(param_1 + 0x1b2) + 0x40] + 0x15c) == '\0')) {
    param_1[0x1ae] = 100;
    return;
  }
  param_1[0x1ae] = 0;
  FUN_10859464(param_1,0x26f);
  return;
}

