/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084c198(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) &&
     (((param_5 == 0 ||
       ((iVar1 = FUN_1086b91c(2), iVar1 == param_3 && (uVar2 = FUN_1086b91c(0x2f), (uVar2 & 1) != 0)
        ))) && (iVar1 = FUN_1084c730(param_2,param_3,param_4), iVar1 != 0)))) {
    FUN_1084c96c(param_1);
    FUN_1084c948(param_2,0,param_3);
    return;
  }
  return;
}

