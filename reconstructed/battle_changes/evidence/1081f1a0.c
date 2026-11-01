/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081f1a0(int param_1,uint *param_2)

{
  uint uVar1;
  byte bVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  bool bVar7;
  
  if ((*param_2 & 0x1fff) >> 8 == 0x1f) {
    iVar5 = 0;
  }
  else {
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
  uVar3 = *(ushort *)((int)param_2 + 0x16);
  bVar7 = (uVar3 & 0x7f) == 0;
  if (bVar7) {
    uVar3 = (ushort)*(byte *)((int)param_2 + 0xd);
  }
  iVar4 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)((int)param_2 + 0xf));
  if (iVar4 != 10) {
    bVar2 = *(byte *)((int)param_2 + 0xf);
    iVar4 = *(int *)(param_1 + 8);
    if (0x17 < bVar2) {
      FUN_10800394(0,0);
    }
    iVar4 = iVar4 + (uint)bVar2 * 4;
    if (*(int *)(iVar4 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar6 = *(undefined4 *)(iVar4 + 0x84);
    iVar4 = FUN_10874420(uVar6,0xd);
    if ((iVar4 != 0) &&
       (iVar4 = FUN_10862858(param_1,uVar6,iVar5,(short)param_2[1],param_2[2],(char)param_2[4],
                             (char)param_2[3] != '\0'), iVar4 == 0)) {
      if ((*param_2 & 0x800000) != 0) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66);
      }
      FUN_108629b8(param_1,uVar6,iVar5,(short)param_2[1],param_2[2],bVar7 && uVar3 == 0,
                   *(char *)((int)param_2 + 0xe) != '\0',param_2 + 5);
      if ((*param_2 & 0x800000) != 0) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar5 + 0x19));
      }
      return 1;
    }
  }
  return 0;
}

