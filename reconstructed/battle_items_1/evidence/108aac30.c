/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aac30(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = FUN_108544a0(param_2,param_3);
  iVar2 = FUN_10856a88(uVar1,0xf);
  if ((iVar2 == 0) && (iVar2 = FUN_1084c730(param_2,param_3,2), iVar2 != 0)) {
    FUN_1084c96c(param_1);
    FUN_1084c948(param_2,0,param_3);
    return;
  }
  return;
}

