/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084cc48(undefined4 param_1,undefined4 param_2,int param_3,uint param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint local_20;
  
  local_20 = param_4;
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) &&
     ((iVar1 = FUN_1087bfa0(0x2f,&local_20), iVar1 == 0 || ((local_20 & 4) != 0)))) {
    uVar2 = FUN_108544a0(param_2,param_3);
    if (param_4 == 0x26) {
      iVar1 = FUN_1086d8ac();
      if (iVar1 == 0) {
        return;
      }
    }
    else if (param_4 == 0x27) {
      iVar1 = FUN_1086d8ac();
      if ((iVar1 == 0) && (iVar1 = FUN_10856a88(uVar2,6), iVar1 == 0)) {
        return;
      }
    }
    else if (param_4 == 0x28) {
      iVar1 = FUN_1084c844();
      if (iVar1 == 0) {
        return;
      }
    }
    else {
      iVar1 = FUN_10856a88(uVar2,param_4);
      if (iVar1 == 0) {
        return;
      }
    }
    FUN_1084c96c(param_1);
    FUN_1084c948(param_2,0,param_3);
  }
  return;
}

