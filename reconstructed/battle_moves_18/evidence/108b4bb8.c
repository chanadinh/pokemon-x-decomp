/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4bb8(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    iVar1 = FUN_1084be58(param_2);
    if (iVar1 == 1) {
      uVar2 = 0xaac;
    }
    else if ((iVar1 == 2 || iVar1 == 3) || (iVar1 == 4)) {
      uVar2 = 0x400;
    }
    else {
      uVar2 = 0x800;
    }
    FUN_108658d4(0x36,uVar2);
    return;
  }
  return;
}

