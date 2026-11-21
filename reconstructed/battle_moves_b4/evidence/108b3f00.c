/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b3f00(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  undefined2 uVar2;
  undefined1 uVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  iVar4 = FUN_1086b91c(3);
  if (iVar4 == param_3) {
    uVar3 = FUN_1086b91c(6);
    uVar5 = FUN_108544a0(param_2,param_3);
    uVar6 = FUN_108544a0(param_2,uVar3);
    iVar4 = FUN_108743a8(uVar5,8);
    iVar7 = FUN_108743a8(uVar6,8);
    iVar8 = FUN_108743a8(uVar5,10);
    iVar9 = FUN_108743a8(uVar6,10);
    iVar10 = FUN_10869f7c(param_2,0x10,param_3);
    uVar1 = (undefined2)((iVar7 + iVar4) / 2);
    *(undefined2 *)(iVar10 + 4) = uVar1;
    *(undefined1 *)(iVar10 + 0xe) = uVar3;
    uVar2 = (undefined2)((iVar9 + iVar8) / 2);
    *(undefined2 *)(iVar10 + 8) = uVar2;
    *(byte *)(iVar10 + 0xf) = *(byte *)(iVar10 + 0xf) | 5;
    FUN_10867b48(param_2);
    iVar4 = FUN_10869f7c(param_2,0x10,param_3);
    *(undefined2 *)(iVar4 + 4) = uVar1;
    *(undefined2 *)(iVar4 + 8) = uVar2;
    *(char *)(iVar4 + 0xe) = (char)param_3;
    *(byte *)(iVar4 + 0xf) = *(byte *)(iVar4 + 0xf) | 5;
    FUN_1084d2d4(iVar4 + 0x10,2,0x44e);
    FUN_1084d29c(iVar4 + 0x10,param_3);
    FUN_10867b48(param_2,iVar4);
    return;
  }
  return;
}

