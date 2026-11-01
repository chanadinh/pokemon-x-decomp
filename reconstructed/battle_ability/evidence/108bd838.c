/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd838(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    uVar3 = FUN_1084c124(uVar2,3);
    uVar4 = FUN_10874420(uVar2,0xd);
    if ((uVar4 <= uVar3) && (iVar1 = FUN_1086b91c(0x16), iVar1 == 10)) {
      FUN_1084ce84(0x36,0x1800);
      return;
    }
  }
  return;
}

