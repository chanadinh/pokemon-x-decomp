/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10863d44(int param_1,uint param_2,uint param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined1 auStack_2c [20];
  
  iVar1 = *(int *)(param_1 + 8);
  if (0x17 < param_2) {
    FUN_10800394(0,0);
  }
  iVar1 = iVar1 + param_2 * 4;
  if (*(int *)(iVar1 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  uVar2 = *(undefined4 *)(iVar1 + 0x84);
  iVar1 = *(int *)(param_1 + 8);
  if (0x17 < param_3) {
    FUN_10800394(0,0);
  }
  iVar1 = iVar1 + param_3 * 4;
  if (*(int *)(iVar1 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar1 = *(int *)(iVar1 + 0x84);
  *(int *)(param_1 + 0x1080) = *(int *)(param_1 + 0x1080) + 1;
  if ((*(byte *)(iVar1 + 0x1b) & 0x40) != 0) {
    iVar1 = FUN_10872ff8(*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),iVar1);
  }
  FUN_10869c5c(param_1,param_4,uVar2,auStack_2c);
  FUN_10826264(param_1,uVar2,iVar1,auStack_2c,0);
  *(int *)(param_1 + 0x1080) = *(int *)(param_1 + 0x1080) + -1;
  return;
}

