/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10822cbc(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  
  bVar1 = *(byte *)(param_2 + 4);
  iVar4 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar4 = iVar4 + (uint)bVar1 * 4;
  if (*(int *)(iVar4 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar5 = *(int *)(iVar4 + 0x84);
  bVar1 = *(byte *)(param_2 + 5);
  iVar4 = *(int *)(param_1 + 8);
  if (0x17 < bVar1) {
    FUN_10800394(0,0);
  }
  iVar4 = iVar4 + (uint)bVar1 * 4;
  if (*(int *)(iVar4 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar4 = *(int *)(iVar4 + 0x84);
  if ((*(uint *)(iVar5 + 0x5c) & 7) != 0) {
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(iVar4 + 0x19));
    DAT_108fca68 = DAT_108fca68 + 1;
    FUN_10864e98(param_1,0x73,7,0);
    DAT_108fca68 = DAT_108fca68 + -1;
    iVar2 = DAT_108fca60;
    if (DAT_108fca68 == 0) {
      for (; iVar2 != 0; iVar2 = *(int *)(iVar2 + 4)) {
        *(undefined2 *)(iVar2 + 0x18) = 0;
      }
    }
    FUN_1086b880();
  }
  FUN_1082aea4(iVar4,iVar5);
  FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x2a,*(undefined1 *)(param_2 + 4),
               *(undefined1 *)(param_2 + 5));
  if (*(short *)(iVar4 + 0x208) != 0) {
    uVar3 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)(param_2 + 5));
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x5d,uVar3);
  }
  return 1;
}

