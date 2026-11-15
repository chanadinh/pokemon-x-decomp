/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8df8(int param_1,undefined4 param_2,int param_3,int param_4)

{
  short sVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    iVar2 = FUN_10869f7c(param_2,10,param_3);
    *(char *)(iVar2 + 5) = (char)param_3;
    *(undefined1 *)(iVar2 + 0x11) = 1;
    *(undefined1 *)(iVar2 + 4) = 0x19;
    FUN_10867b48(param_2);
    *(undefined4 *)(param_4 + 0x18) = 0;
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
  }
  return;
}

