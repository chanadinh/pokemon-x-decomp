/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4ef8(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined1 *puVar4;
  int iVar5;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    iVar2 = FUN_10856a88(uVar3,0x12);
    if (iVar2 != 0) {
      iVar2 = FUN_10869f7c(param_2,10,param_3);
      *(undefined1 *)(iVar2 + 0x11) = 1;
      *(char *)(iVar2 + 5) = (char)param_3;
      *(undefined1 *)(iVar2 + 4) = 0x12;
      FUN_10867b48(param_2);
    }
    iVar2 = FUN_10856a88(uVar3,8);
    if (iVar2 != 0) {
      iVar2 = FUN_10869f7c(param_2,10,param_3);
      *(undefined1 *)(iVar2 + 0x11) = 1;
      *(char *)(iVar2 + 5) = (char)param_3;
      *(undefined1 *)(iVar2 + 4) = 8;
      FUN_10867b48(param_2,iVar2);
    }
    iVar2 = FUN_10869f7c(param_2,0x1a,param_3);
    uVar1 = FUN_1084bfc0(param_3);
    *(undefined1 *)(iVar2 + 8) = uVar1;
    *(undefined1 *)(iVar2 + 4) = 4;
    puVar4 = (undefined1 *)(iVar2 + 5);
    iVar5 = 1;
    *(undefined1 *)(iVar2 + 5) = 0;
    do {
      puVar4[1] = 0;
      iVar5 = iVar5 + -1;
      puVar4 = puVar4 + 2;
      *puVar4 = 0;
    } while (iVar5 != 0);
    if (1 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 5) = *(byte *)(iVar2 + 5) | 0x40;
    }
    if (1 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 5) = *(byte *)(iVar2 + 5) | 0x80;
    }
    if (2 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 6) = *(byte *)(iVar2 + 6) | 1;
    }
    if (2 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 6) = *(byte *)(iVar2 + 6) | 0x40;
    }
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

