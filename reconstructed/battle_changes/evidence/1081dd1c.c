/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081dd1c(int param_1,uint *param_2)

{
  uint uVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar3 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)((int)param_2 + 6));
  if (iVar3 != 10) {
    bVar2 = *(byte *)((int)param_2 + 6);
    iVar3 = *(int *)(param_1 + 8);
    if (0x17 < bVar2) {
      FUN_10800394(0,0);
    }
    iVar3 = iVar3 + (uint)bVar2 * 4;
    if (*(int *)(iVar3 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar3 = *(int *)(iVar3 + 0x84);
    iVar5 = 0;
    if ((*param_2 & 0x1fff) >> 8 != 0x1f) {
      iVar5 = *(int *)(param_1 + 8);
      uVar1 = (*param_2 & 0x1fff) >> 8;
      if (0x17 < uVar1) {
        FUN_10800394(0,0);
      }
      iVar5 = iVar5 + uVar1 * 4;
      if (*(int *)(iVar5 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      iVar5 = *(int *)(iVar5 + 0x84);
    }
    iVar4 = FUN_10874420(iVar3,0xd);
    if ((iVar4 != 0) &&
       (((*(byte *)((int)param_2 + 7) & 1) == 0 || (iVar4 = FUN_10874348(iVar3), iVar4 == 0x12)))) {
      FUN_1086bb84();
      FUN_1086bac0(4,*(undefined1 *)(iVar3 + 0x19));
      FUN_1086b9fc(0x53,1);
      FUN_1086b9a4(param_1,0x89);
      iVar4 = FUN_1086b91c(0x53);
      if (iVar4 != 0) {
        FUN_1086b880();
        if ((*param_2 & 0x800000) != 0) {
          FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar5 + 0x19));
        }
        if ((*(byte *)((int)param_2 + 7) & 2) != 0) {
          FUN_1086a6e0(param_1,(short)param_2[2],*(undefined1 *)((int)param_2 + 10),
                       *(undefined1 *)((int)param_2 + 0xb),0,0);
        }
        FUN_108671c0(param_1,iVar3,(short)param_2[1],param_2 + 3);
        if ((*param_2 & 0x800000) != 0) {
          FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar5 + 0x19));
        }
        return 1;
      }
      FUN_1086b880();
    }
  }
  return 0;
}

