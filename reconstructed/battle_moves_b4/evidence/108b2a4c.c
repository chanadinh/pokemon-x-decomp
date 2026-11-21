/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b2a4c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(6);
    uVar3 = FUN_1081d83c(param_2);
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(undefined1 *)(iVar2 + 4) = 1;
    *(undefined1 *)(iVar2 + 0x17) = 0xfe;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 0x16;
    *(undefined4 *)(iVar2 + 8) = uVar3;
    FUN_10867b48(param_2);
    iVar2 = FUN_10869f7c(param_2,0xd,param_3);
    *(undefined1 *)(iVar2 + 0x16) = 1;
    *(undefined1 *)(iVar2 + 4) = 3;
    *(undefined1 *)(iVar2 + 0x17) = 0xfe;
    *(undefined1 *)(iVar2 + 0xc) = uVar1;
    *(undefined4 *)(iVar2 + 8) = uVar3;
    *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 0x16;
    FUN_10867b48(param_2);
    iVar2 = FUN_10869f7c(param_2,0x12,param_3);
    *(char *)(iVar2 + 4) = (char)param_3;
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

