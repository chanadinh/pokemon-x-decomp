/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bb5e8(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,2,0x22c);
    FUN_1084d29c(iVar1 + 4,param_3);
    FUN_10867b48(param_2,iVar1);
    iVar1 = FUN_10869f7c(param_2,0xd,param_3);
    *(char *)(iVar1 + 0xc) = (char)param_3;
    *(undefined1 *)(iVar1 + 0x16) = 1;
    *(undefined1 *)(iVar1 + 4) = 2;
    *(undefined1 *)(iVar1 + 0x17) = 1;
    *(byte *)(iVar1 + 0x18) = *(byte *)(iVar1 + 0x18) | 6;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

