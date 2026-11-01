/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10821d98(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)(param_2 + 0xe),param_3,param_4,param_4);
  if (iVar2 == 10) {
    uVar3 = 0;
  }
  else {
    bVar1 = *(byte *)(param_2 + 0xe);
    iVar2 = *(int *)(param_1 + 8);
    if (0x17 < bVar1) {
      FUN_10800394(0,0);
    }
    iVar2 = iVar2 + (uint)bVar1 * 4;
    if (*(int *)(iVar2 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar3 = *(undefined4 *)(iVar2 + 0x84);
    if ((*(byte *)(param_2 + 0xf) & 1) != 0) {
      FUN_10871250(uVar3,8,*(undefined2 *)(param_2 + 4));
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x14,*(undefined1 *)(param_2 + 0xe),8,
                   *(ushort *)(param_2 + 4) & 0xff);
    }
    if ((*(byte *)(param_2 + 0xf) & 2) != 0) {
      FUN_10871250(uVar3,9,*(undefined2 *)(param_2 + 6));
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x14,*(undefined1 *)(param_2 + 0xe),9,
                   *(ushort *)(param_2 + 6) & 0xff);
    }
    if ((*(byte *)(param_2 + 0xf) & 4) != 0) {
      FUN_10871250(uVar3,10,*(undefined2 *)(param_2 + 8));
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x14,*(undefined1 *)(param_2 + 0xe),10,
                   *(ushort *)(param_2 + 8) & 0xff);
    }
    if ((*(byte *)(param_2 + 0xf) & 8) != 0) {
      FUN_10871250(uVar3,0xb,*(undefined2 *)(param_2 + 10));
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x14,*(undefined1 *)(param_2 + 0xe),0xb,
                   *(ushort *)(param_2 + 10) & 0xff);
    }
    if ((*(byte *)(param_2 + 0xf) & 0x10) != 0) {
      FUN_10871250(uVar3,0xc,*(undefined2 *)(param_2 + 0xc));
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x14,*(undefined1 *)(param_2 + 0xe),0xc,
                   *(ushort *)(param_2 + 0xc) & 0xff);
    }
    FUN_1086bbe8(param_1,param_2 + 0x10);
    uVar3 = 1;
  }
  return uVar3;
}

