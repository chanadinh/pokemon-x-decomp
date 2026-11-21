/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b402c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    iVar2 = FUN_10869f7c(param_2,0x10,param_3);
    *(char *)(iVar2 + 0xe) = (char)param_3;
    uVar1 = FUN_108743a8(uVar3,9);
    *(undefined2 *)(iVar2 + 4) = uVar1;
    uVar1 = FUN_108743a8(uVar3,8);
    *(undefined2 *)(iVar2 + 6) = uVar1;
    *(byte *)(iVar2 + 0xf) = *(byte *)(iVar2 + 0xf) | 3;
    FUN_1084d2d4(iVar2 + 0x10,2,0x305);
    FUN_1084d29c(iVar2 + 0x10,param_3);
    FUN_10867b48(param_2,iVar2);
    iVar2 = FUN_10869f7c(param_2,0x17,param_3);
    *(char *)(iVar2 + 5) = (char)param_3;
    *(undefined1 *)(iVar2 + 4) = 10;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

