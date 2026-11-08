/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b682c(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    if (*(short *)(param_1 + 0x38) == 0x22e) {
      iVar1 = 0x22f;
    }
    else {
      if (*(short *)(param_1 + 0x38) != 0x22f) {
        return;
      }
      iVar1 = 0x22e;
    }
    uVar2 = FUN_1084d364(param_2);
    iVar3 = FUN_108c9b28(param_2 + 0x20,uVar2);
    if (iVar3 == iVar1) {
      FUN_1084ce84(0x32,0x2000);
      FUN_1084bef8(param_2,1);
      return;
    }
  }
  return;
}

