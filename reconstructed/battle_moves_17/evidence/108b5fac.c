/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5fac(int param_1,undefined4 param_2,int param_3)

{
  ushort uVar1;
  undefined1 uVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 != param_3) {
    return;
  }
  iVar3 = FUN_1086b91c(0x1e);
  if (iVar3 == 8) {
    uVar4 = FUN_1086b91c(0x40);
    uVar2 = FUN_1086b91c(4);
    uVar1 = *(ushort *)(param_1 + 0x38);
    if (uVar1 != 0xfa) {
      if (uVar1 < 0xfb) {
        if (((uVar1 != 0x14) && (uVar1 != 0x23)) && ((uVar1 != 0x53 && (uVar1 != 0x80)))) {
          return;
        }
      }
      else if (((uVar1 != 0x148) && (uVar1 != 0x1cf)) && (uVar1 != 0x263)) {
        return;
      }
    }
    FUN_1084d2d4(uVar4,2);
    FUN_1084d29c(uVar4,uVar2);
    FUN_1084d29c(uVar4,param_3);
    return;
  }
  return;
}

