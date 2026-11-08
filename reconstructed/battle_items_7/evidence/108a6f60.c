/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a6f60(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined4 uVar6;
  uint uVar7;
  bool bVar8;
  bool bVar9;
  
  iVar3 = FUN_1086b91c(2);
  if ((iVar3 != param_3) || (iVar3 = FUN_10865c48(param_2), iVar3 != 0)) {
    return;
  }
  uVar4 = FUN_108544a0(param_2,param_3);
  uVar5 = FUN_10870994();
  uVar7 = uVar5 & 0x1f;
  bVar8 = uVar7 != 3;
  if (bVar8) {
    uVar7 = uVar5 << 0x16;
  }
  bVar9 = uVar7 >> 0x1b != 3;
  if (bVar8 && bVar9) {
    uVar5 = uVar5 << 0x11;
  }
  if ((!bVar8 || !bVar9) || uVar5 >> 0x1b == 3) {
    iVar3 = FUN_10869f7c(param_2,4,param_3);
    *(char *)(iVar3 + 6) = (char)param_3;
    uVar6 = FUN_1084cf74(*(undefined2 *)(param_1 + 0x38),3);
    uVar2 = FUN_1084d2f4(uVar4,uVar6);
    *(undefined2 *)(iVar3 + 4) = uVar2;
    FUN_1084d2d4(iVar3 + 8,2,0x398);
    FUN_1084d29c(iVar3 + 8,param_3);
    FUN_1084d29c(iVar3 + 8,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar3);
    return;
  }
  iVar3 = FUN_10869f7c(param_2,6,param_3);
  *(char *)(iVar3 + 6) = (char)param_3;
  uVar2 = FUN_1084d2f4(uVar4,8);
  *(undefined2 *)(iVar3 + 4) = uVar2;
  *(undefined2 *)(iVar3 + 8) = 0x26f;
  *(byte *)(iVar3 + 7) = *(byte *)(iVar3 + 7) | 2;
  uVar1 = FUN_10873158(param_2,param_3);
  *(undefined1 *)(iVar3 + 10) = uVar1;
  *(undefined1 *)(iVar3 + 0xb) = 10;
  FUN_1084d2d4(iVar3 + 0xc,2,0x414);
  FUN_1084d29c(iVar3 + 0xc,param_3);
  FUN_1084d29c(iVar3 + 0xc,*(undefined2 *)(param_1 + 0x38));
  FUN_10867b48(param_2,iVar3);
  return;
}

