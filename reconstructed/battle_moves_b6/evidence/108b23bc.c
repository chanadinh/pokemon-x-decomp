/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b23bc(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 *puVar3;
  int iVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(6);
    FUN_108544a0(param_2,uVar1);
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 4) = 7;
    *(undefined1 *)(iVar2 + 0x17) = 0xff;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 0x26;
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    FUN_10867b48(param_2);
    iVar2 = FUN_10869f7c(param_2,0x1a,param_3);
    uVar1 = FUN_1084bfc0(uVar1);
    *(undefined1 *)(iVar2 + 8) = uVar1;
    *(undefined1 *)(iVar2 + 4) = 4;
    puVar3 = (undefined1 *)(iVar2 + 5);
    iVar4 = 1;
    *(undefined1 *)(iVar2 + 5) = 0;
    do {
      puVar3[1] = 0;
      iVar4 = iVar4 + -1;
      puVar3 = puVar3 + 2;
      *puVar3 = 0;
    } while (iVar4 != 0);
    if (1 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 5) = *(byte *)(iVar2 + 5) | 1;
    }
    if (1 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 5) = *(byte *)(iVar2 + 5) | 2;
    }
    if (1 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 5) = *(byte *)(iVar2 + 5) | 8;
    }
    if (1 < *(byte *)(iVar2 + 4)) {
      *(byte *)(iVar2 + 5) = *(byte *)(iVar2 + 5) | 4;
    }
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
    iVar2 = FUN_10869f7c(param_2,0x1a,param_3);
    uVar1 = FUN_1084bfc0(param_3);
    *(undefined1 *)(iVar2 + 8) = uVar1;
    *(undefined1 *)(iVar2 + 4) = 4;
    puVar3 = (undefined1 *)(iVar2 + 5);
    iVar4 = 1;
    *(undefined1 *)(iVar2 + 5) = 0;
    do {
      puVar3[1] = 0;
      iVar4 = iVar4 + -1;
      puVar3 = puVar3 + 2;
      *puVar3 = 0;
    } while (iVar4 != 0);
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

