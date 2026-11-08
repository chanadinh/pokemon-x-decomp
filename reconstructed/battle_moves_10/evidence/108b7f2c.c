/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7f2c(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  short sVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(2);
  if ((iVar3 == param_3) && (cVar1 = FUN_1086b91c(0x23), cVar1 == '\0')) {
    uVar4 = FUN_108544a0(param_2,param_3);
    iVar3 = FUN_10856a88(uVar4,2);
    if (iVar3 != 0) {
      FUN_108658d4(0x23,0x1b);
      return;
    }
    iVar3 = FUN_10856a88(uVar4,0xf);
    if (iVar3 != 0) {
      FUN_108658d4(0x23,0xd);
      return;
    }
    iVar3 = FUN_1084d318(uVar4);
    if (iVar3 != 0) {
      FUN_108658d4(0x23,0xe);
      return;
    }
    sVar2 = FUN_10874420(uVar4,0x11);
    if ((sVar2 == 0xf) || (sVar2 == 0x48)) {
      FUN_108658d4(0x23,0xf);
      return;
    }
  }
  return;
}

