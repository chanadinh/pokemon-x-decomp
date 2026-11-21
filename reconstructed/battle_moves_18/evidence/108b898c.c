/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b898c(int param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = FUN_1086b91c(2);
  if (iVar3 == param_3) {
    iVar3 = FUN_108544a0(param_2,param_3);
    iVar4 = FUN_10856a88(iVar3,2);
    if (iVar4 != 0) {
      iVar4 = FUN_10869f7c(param_2,10,param_3);
      *(undefined1 *)(iVar4 + 4) = 0x19;
      *(char *)(iVar4 + 5) = (char)param_3;
      *(undefined1 *)(iVar4 + 0x11) = 1;
      FUN_10867b48(param_2);
      sVar2 = *(short *)(param_1 + 0x38);
      uVar1 = *(undefined1 *)(iVar3 + 0x19);
      iVar3 = FUN_1084d3c0(0,uVar1);
      while (iVar3 != 0) {
        if (*(short *)(iVar3 + 0x38) == sVar2) {
          FUN_1084d1cc();
          iVar3 = FUN_1084d3c0(0,uVar1);
        }
        else {
          iVar3 = FUN_1084d370(iVar3);
        }
      }
    }
  }
  return;
}

