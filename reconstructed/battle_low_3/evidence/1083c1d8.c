/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083c1d8(int param_1,int param_2,undefined1 param_3,undefined4 param_4)

{
  byte bVar1;
  int *piVar2;
  int iVar3;
  
  if (DAT_108fcb84 != 0x102f) {
    DAT_108fcb84 = 0x102f;
  }
  FUN_1086fd18((undefined1 *)(param_1 + 0x6ca),0xff);
  *(undefined1 *)(param_1 + 0x25d) = 0;
  *(char *)(param_1 + 0x6c8) = (char)param_2;
  *(undefined1 *)(param_1 + 0x6c9) = param_3;
  *(undefined1 *)(param_1 + 0x6ca) = param_3;
  *(undefined4 *)(param_1 + 0x6b8) = 0;
  *(undefined4 *)(param_1 + 0x6e0) = param_4;
  piVar2 = *(int **)(param_1 + param_2 * 4 + 0x100);
  if ((piVar2 == (int *)0x0) || (iVar3 = (**(code **)(*piVar2 + 0x7c))(), iVar3 == 0)) {
    *(undefined4 *)(param_1 + 0x6b8) = 0xff;
    goto LAB_1083c2b4;
  }
  bVar1 = *(byte *)(param_1 + 0x6c8);
  iVar3 = (**(code **)(*(int *)**(undefined4 **)(param_1 + 400) + 0x44))();
  if (iVar3 == 4) {
    if (bVar1 == 0) goto LAB_1083c294;
LAB_1083c280:
    *(undefined1 *)(param_1 + 0x25d) = 1;
  }
  else {
    if ((bVar1 & 1) != 0) goto LAB_1083c280;
LAB_1083c294:
    *(undefined1 *)(param_1 + 0x25d) = 0;
  }
  FUN_10859464(param_1,*(undefined4 *)(param_1 + 0x6e0));
LAB_1083c2b4:
  if (DAT_108fcb84 != 0x1051) {
    DAT_108fcb84 = 0x1051;
  }
  return;
}

