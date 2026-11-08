/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b9080(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_108544ec(param_2);
  if ((iVar1 != 0) || (iVar1 = FUN_1084bf14(param_2), iVar1 != 0)) {
    FUN_108658d4(0x23,0x1b);
  }
  iVar1 = FUN_1081d770(param_2,param_3);
  if (iVar1 == 0) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_10856a88(uVar2,8);
    if (((iVar1 != 0) || (iVar1 = FUN_10856a88(uVar2,0x16), iVar1 != 0)) ||
       (iVar1 = FUN_1084c374(param_2,9), iVar1 != 0)) {
      FUN_108658d4(0x23,0x1b);
      return;
    }
  }
  return;
}

