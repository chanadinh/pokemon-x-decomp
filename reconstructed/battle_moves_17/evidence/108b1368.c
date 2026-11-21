/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b1368(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    FUN_108544a0(param_2,param_3);
    uVar3 = FUN_108c8754();
    if (1 < uVar3) {
      iVar2 = FUN_10869f7c(param_2,0x2e,param_3);
      *(char *)(iVar2 + 4) = (char)param_3;
      if (uVar3 < 0x3ea) {
        sVar1 = 1;
      }
      else {
        sVar1 = (short)uVar3 + -1000;
      }
      *(short *)(iVar2 + 6) = sVar1;
      FUN_1084d2d4(iVar2 + 8,2,0x454);
      FUN_1084d29c(iVar2 + 8,param_3);
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

