/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081d558(int param_1,int param_2)

{
  byte bVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  
  uVar2 = (uint)*(byte *)(param_2 + 7);
  uVar4 = 0;
  if (uVar2 != 0x1f) {
    iVar5 = *(int *)(param_1 + 8);
    if (0x17 < uVar2) {
      FUN_10800394(0,0);
    }
    iVar5 = iVar5 + uVar2 * 4;
    if (*(int *)(iVar5 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar4 = *(undefined4 *)(iVar5 + 0x84);
  }
  iVar5 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)(param_2 + 6));
  if (iVar5 != 10) {
    bVar1 = *(byte *)(param_2 + 6);
    iVar5 = *(int *)(param_1 + 8);
    if (0x17 < bVar1) {
      FUN_10800394(0,0);
    }
    iVar5 = iVar5 + (uint)bVar1 * 4;
    if (*(int *)(iVar5 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar3 = *(undefined4 *)(iVar5 + 0x84);
    iVar5 = FUN_10874420(uVar3,0xd);
    if ((iVar5 != 0) &&
       (iVar5 = FUN_10863f5c(param_1,uVar3,uVar4,*(undefined2 *)(param_2 + 4)), iVar5 != 0)) {
      if ((*(ushort *)(param_2 + 10) & 0x7f) != 0) {
        FUN_1086bbe8(param_1,param_2 + 8);
      }
      return 1;
    }
  }
  return 0;
}

