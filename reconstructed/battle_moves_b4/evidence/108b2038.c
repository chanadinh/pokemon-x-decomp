/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b2038(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  undefined1 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar2 = FUN_1086b91c(6);
    uVar4 = FUN_108544a0(param_2,param_3);
    uVar5 = FUN_108544a0(param_2,uVar2);
    iVar3 = FUN_10874420(uVar4,0xd);
    iVar6 = FUN_10874420(uVar5,0xd);
    uVar1 = (uint)(iVar3 + iVar6) >> 1;
    iVar7 = FUN_10869f7c(param_2,7,param_3);
    *(undefined1 *)(iVar7 + 4) = 2;
    *(uint *)(iVar7 + 0x14) = uVar1 - iVar3;
    *(uint *)(iVar7 + 0x18) = uVar1 - iVar6;
    *(undefined1 *)(iVar7 + 6) = 1;
    *(char *)(iVar7 + 7) = (char)param_3;
    *(undefined1 *)(iVar7 + 8) = uVar2;
    FUN_10867b48(param_2);
    iVar3 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar3 + 4,1,0x75);
    FUN_10867b48(param_2,iVar3);
    iVar3 = FUN_10869f7c(param_2,0x22,param_3);
    *(char *)(iVar3 + 4) = (char)param_3;
    *(undefined1 *)(iVar3 + 5) = 1;
    FUN_10867b48(param_2,iVar3);
    iVar3 = FUN_10869f7c(param_2,0x22,param_3);
    *(undefined1 *)(iVar3 + 4) = uVar2;
    *(undefined1 *)(iVar3 + 5) = 1;
    FUN_10867b48(param_2,iVar3);
    return;
  }
  return;
}

