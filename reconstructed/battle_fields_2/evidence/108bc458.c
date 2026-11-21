/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc458(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  
  uVar1 = FUN_1086b91c(4);
  uVar2 = FUN_108544a0(param_2,uVar1);
  iVar3 = FUN_1084ca4c(uVar2,0x10);
  if (iVar3 != 0) {
    iVar3 = FUN_10869f7c(param_2,0x18,uVar1);
    *(undefined1 *)(iVar3 + 4) = 0x10;
    *(undefined1 *)(iVar3 + 5) = uVar1;
    FUN_10867b48(param_2,iVar3);
    iVar4 = FUN_10869f7c(param_2,3,uVar1);
    FUN_1084d2d4(iVar4 + 4,2,0x4b7);
    FUN_1084d29c(iVar4 + 4,uVar1);
    FUN_10867b48(param_2,iVar3);
    return;
  }
  return;
}

