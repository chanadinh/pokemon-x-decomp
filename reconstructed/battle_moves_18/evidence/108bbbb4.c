/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bbbb4(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    iVar2 = FUN_108544a0(param_2,param_3);
    sVar1 = *(short *)(iVar2 + 0x12);
    if (sVar1 != 0) {
      iVar2 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar2 + 4,2,0x30b);
      FUN_1084d29c(iVar2 + 4,param_3);
      FUN_1084d29c(iVar2 + 4,sVar1);
      FUN_10867b48(param_2,iVar2);
      iVar2 = FUN_10869f7c(param_2,0x15,param_3);
      *(char *)(iVar2 + 5) = (char)param_3;
      *(undefined1 *)(iVar2 + 4) = 0xf;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

