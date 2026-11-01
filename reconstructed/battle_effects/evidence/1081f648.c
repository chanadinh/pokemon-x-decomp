/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081f648(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)(param_2 + 4));
  if (iVar2 != 10) {
    bVar1 = *(byte *)(param_2 + 4);
    iVar2 = *(int *)(param_1 + 8);
    if (0x17 < bVar1) {
      FUN_10800394(0,0);
    }
    iVar2 = iVar2 + (uint)bVar1 * 4;
    if (*(int *)(iVar2 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar3 = *(int *)(iVar2 + 0x84);
    iVar2 = FUN_10874420(iVar3,0xd);
    if (iVar2 != 0) {
      FUN_108714fc(iVar3,1,*(undefined1 *)(param_2 + 5));
      FUN_108714fc(iVar3,2,*(undefined1 *)(param_2 + 6));
      FUN_108714fc(iVar3,3,*(undefined1 *)(param_2 + 7));
      FUN_108714fc(iVar3,4,*(undefined1 *)(param_2 + 8));
      FUN_108714fc(iVar3,5,*(undefined1 *)(param_2 + 9));
      FUN_108714fc(iVar3,6,*(undefined1 *)(param_2 + 10));
      FUN_108714fc(iVar3,7,*(undefined1 *)(param_2 + 0xb));
      if (*(byte *)(param_2 + 0xc) < 4) {
        *(byte *)(iVar3 + 0x155) = *(byte *)(param_2 + 0xc);
      }
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0xc,*(undefined1 *)(param_2 + 4),
                   *(undefined1 *)(param_2 + 5),*(undefined1 *)(param_2 + 6),
                   *(undefined1 *)(param_2 + 7),*(undefined1 *)(param_2 + 8),
                   *(undefined1 *)(param_2 + 9),*(undefined1 *)(param_2 + 10),
                   *(undefined1 *)(param_2 + 0xb),*(undefined1 *)(param_2 + 0xc));
      return 1;
    }
  }
  return 0;
}

