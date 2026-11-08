/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8fd4(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    uVar3 = FUN_1086b91c(0x40);
    uVar1 = FUN_1086b91c(0x12);
    FUN_1084d2d4(uVar3,1,0x7f);
    FUN_1084d29c(uVar3,uVar1);
    FUN_108658d4(0x53,1);
    return;
  }
  return;
}

