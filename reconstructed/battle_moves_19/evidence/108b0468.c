/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b0468(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  undefined1 uVar2;
  undefined1 uVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = FUN_1086b91c(3);
  if (iVar4 == param_3) {
    uVar2 = FUN_1086b91c(6);
    iVar4 = FUN_108544a0(param_2,uVar2);
    iVar5 = FUN_10869f7c(param_2,0xe,param_3);
    uVar1 = (undefined1)param_3;
    *(undefined1 *)(iVar5 + 4) = uVar1;
    uVar3 = FUN_10874420(iVar4,1);
    *(undefined1 *)(iVar5 + 5) = uVar3;
    uVar3 = FUN_10874420(iVar4,2);
    *(undefined1 *)(iVar5 + 6) = uVar3;
    uVar3 = FUN_10874420(iVar4,3);
    *(undefined1 *)(iVar5 + 7) = uVar3;
    uVar3 = FUN_10874420(iVar4,4);
    *(undefined1 *)(iVar5 + 8) = uVar3;
    uVar3 = FUN_10874420(iVar4,5);
    *(undefined1 *)(iVar5 + 9) = uVar3;
    uVar3 = FUN_10874420(iVar4,6);
    *(undefined1 *)(iVar5 + 10) = uVar3;
    uVar3 = FUN_10874420(iVar4,7);
    *(undefined1 *)(iVar5 + 0xb) = uVar3;
    *(undefined1 *)(iVar5 + 0xc) = *(undefined1 *)(iVar4 + 0x155);
    FUN_10867b48(param_2,iVar5);
    iVar4 = FUN_1084ca4c(iVar4,9);
    if (iVar4 == 0) {
      iVar4 = FUN_10869f7c(param_2,0x18,param_3);
      *(undefined1 *)(iVar4 + 5) = uVar1;
      *(undefined1 *)(iVar4 + 4) = 9;
      FUN_10867b48(param_2,iVar4);
    }
    else {
      iVar4 = FUN_10869f7c(param_2,0x17,param_3);
      *(undefined1 *)(iVar4 + 5) = uVar1;
      *(undefined1 *)(iVar4 + 4) = 9;
      FUN_10867b48(param_2,iVar4);
    }
    iVar4 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar4 + 4,2,0x41d);
    FUN_1084d29c(iVar4 + 4,param_3);
    FUN_1084d29c(iVar4 + 4,uVar2);
    FUN_10867b48(param_2,iVar4);
    return;
  }
  return;
}

