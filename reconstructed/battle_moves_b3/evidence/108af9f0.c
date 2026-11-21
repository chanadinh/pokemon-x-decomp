/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108af9f0(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (iVar2 = FUN_108544a0(param_2,param_3), *(short *)(iVar2 + 0x12) == 0))
  {
    sVar1 = *(short *)(iVar2 + 0x14);
    if (sVar1 != 0) {
      iVar2 = FUN_10869f7c(param_2,0x21,param_3);
      *(char *)(iVar2 + 6) = (char)param_3;
      *(short *)(iVar2 + 4) = sVar1;
      *(undefined1 *)(iVar2 + 7) = 1;
      FUN_1084d2d4(iVar2 + 0xc,2,0x2dd);
      FUN_1084d29c(iVar2 + 0xc,param_3);
      FUN_1084d29c(iVar2 + 0xc,sVar1);
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

