/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5574(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_1086b91c(0x40);
    FUN_108658d4(0x1e,0x20);
    FUN_1084d2d4(uVar2,2,0x47a);
    uVar3 = FUN_1086b91c(4);
    FUN_1084d29c(uVar2,uVar3);
    return;
  }
  return;
}

