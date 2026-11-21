/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5cc4(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 4) = 2;
    *(undefined1 *)(iVar2 + 0x17) = 0xff;
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 6;
    uVar1 = (undefined1)param_3;
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    FUN_10867b48(param_2);
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 4) = 4;
    *(undefined1 *)(iVar2 + 0x17) = 0xff;
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 6;
    FUN_10867b48(param_2,iVar2);
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 4) = 1;
    *(undefined1 *)(iVar2 + 0x17) = 2;
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 6;
    FUN_10867b48(param_2);
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 4) = 3;
    *(undefined1 *)(iVar2 + 0x17) = 2;
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 6;
    FUN_10867b48(param_2);
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 0x17) = 2;
    *(undefined1 *)(iVar2 + 4) = 5;
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 6;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

