/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7a20(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1084bf14(param_2);
  if ((iVar1 != 0) && (iVar1 = FUN_1084bf14(param_2), iVar1 != 3)) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084bb2c(uVar2,4);
    if ((iVar1 != 0) && ((iVar1 - 1U & 0xff) < 0x18)) {
      FUN_108658d4(4);
      return;
    }
  }
  return;
}

