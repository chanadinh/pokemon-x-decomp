/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

/* WARNING: Removing unreachable block (ram,0x108574d0) */

void FUN_1083b110(int *param_1,int param_2,undefined1 param_3)

{
  int iVar1;
  
  if (DAT_108fcb84 != 0x11f1) {
    DAT_108fcb84 = 0x11f1;
  }
  FUN_1086fd18((undefined1 *)((int)param_1 + 0x6ca),0xff);
  *(undefined1 *)((int)param_1 + 0x25d) = 0;
  *(char *)(param_1 + 0x1b2) = (char)param_2;
  *(undefined1 *)((int)param_1 + 0x6c9) = param_3;
  *(undefined1 *)((int)param_1 + 0x6ca) = param_3;
  param_1[0x1b8] = 0x273;
  *(undefined1 *)(param_1 + 0x1c9) = *(undefined1 *)(param_1[param_2 + 0x40] + 0x15c);
  iVar1 = (**(code **)(*param_1 + 0xbc))(param_1);
  if ((iVar1 == 0) && (iVar1 = (**(code **)(*(int *)param_1[param_2 + 0x40] + 0x7c))(), iVar1 != 0))
  {
    param_1[0x1ae] = 0;
  }
  else {
    param_1[0x1ae] = 100;
  }
  return;
}

