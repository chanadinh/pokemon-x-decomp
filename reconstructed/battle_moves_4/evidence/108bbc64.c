/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bbc64(int param_1,undefined4 param_2,undefined4 param_3)

{
  short sVar1;
  int iVar2;
  
  sVar1 = *(short *)(param_1 + 0x38);
  iVar2 = FUN_1084d3c0(0,param_3);
  while (iVar2 != 0) {
    if (*(short *)(iVar2 + 0x38) == sVar1) {
      FUN_1084d1cc();
      iVar2 = FUN_1084d3c0(0,param_3);
    }
    else {
      iVar2 = FUN_1084d370(iVar2);
    }
  }
  return;
}

