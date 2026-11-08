/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aaa9c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(4);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x47), iVar1 == 0)) &&
     (iVar1 = FUN_1086b91c(0x16), iVar1 == 10)) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_108722e8(uVar2,3,1);
    if (iVar1 != 0) {
      FUN_1084c96c(param_1);
      FUN_1084c948(param_2,0,param_3);
      return;
    }
  }
  return;
}

