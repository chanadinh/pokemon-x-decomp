/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc158(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  bool bVar4;
  
  iVar3 = FUN_1086b91c(0x16);
  if (iVar3 == 0xb) {
    uVar1 = FUN_1086b91c(3);
    iVar3 = FUN_1084bf24(param_2,uVar1);
    if ((iVar3 == 0) && (iVar3 = FUN_1084af30(param_2,uVar1), iVar3 == 0)) {
      FUN_1084ce84(0x32,0x1800);
    }
  }
  sVar2 = FUN_1086b91c(0x12);
  bVar4 = sVar2 == 0x59 || sVar2 == 0xde;
  if (sVar2 != 0x59 && sVar2 != 0xde) {
    bVar4 = sVar2 == 0x20b;
  }
  if (bVar4) {
    uVar1 = FUN_1086b91c(4);
    iVar3 = FUN_1084bf24(param_2,uVar1);
    if ((iVar3 == 0) && (iVar3 = FUN_1084af30(param_2,uVar1), iVar3 == 0)) {
      FUN_1084ce84(0x32,0x800);
      return;
    }
  }
  return;
}

