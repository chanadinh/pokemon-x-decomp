/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c30a4(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 *param_4)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  uVar1 = FUN_1086b91c(2);
  uVar1 = uVar1 & 0xff;
  iVar2 = FUN_1084c5a8(param_3,uVar1);
  if (iVar2 != 0) {
    uVar3 = FUN_108544a0(param_2,uVar1);
    iVar2 = FUN_1086f960(uVar3,0xb);
    if ((((iVar2 != 0) && (uVar4 = FUN_1086b91c(2), uVar4 == uVar1)) &&
        (uVar4 = FUN_1086b91c(3), uVar4 != uVar1)) && (iVar2 = FUN_1086b91c(0x21), iVar2 < 0)) {
      uVar3 = FUN_108658d4(0x42,1);
      *param_4 = uVar3;
    }
  }
  return;
}

