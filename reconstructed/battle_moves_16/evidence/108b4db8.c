/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4db8(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x47), iVar1 == 0)) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1085f844();
    if (iVar1 == 0) {
      iVar1 = FUN_10869f7c(param_2,0xd,param_3);
      *(char *)(iVar1 + 0xc) = (char)param_3;
      *(undefined1 *)(iVar1 + 4) = 1;
      *(undefined1 *)(iVar1 + 0x17) = 1;
      *(undefined1 *)(iVar1 + 0x16) = 1;
      *(byte *)(iVar1 + 0x18) = *(byte *)(iVar1 + 0x18) & 0xfd | 5;
      FUN_1084d2d4(iVar1 + 0x1c,2,0x214);
      FUN_1084d29c(iVar1 + 0x1c,param_3);
      FUN_10867b48(param_2,iVar1);
      return;
    }
  }
  return;
}

