/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b1f70(int param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  
  iVar3 = FUN_1086b91c(2);
  if (iVar3 == param_3) {
    uVar4 = FUN_1086b91c(0x12);
    if (uVar4 == *(ushort *)(param_1 + 0x38)) {
      *(undefined4 *)(param_4 + 0x18) = 1;
    }
    else {
      iVar3 = FUN_108544a0(param_2,param_3);
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

