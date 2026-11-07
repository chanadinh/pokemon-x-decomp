/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084c4c0(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 != param_3) {
    return;
  }
  uVar2 = FUN_108544a0(param_2,param_3);
  if (param_4 == 0x26) {
    iVar1 = FUN_1086d8ac();
    if (iVar1 == 0) {
      return;
    }
  }
  else {
    if (param_4 == 0x27) {
      iVar1 = FUN_1086d8ac();
      if (iVar1 != 0) goto LAB_1084c51c;
      iVar1 = FUN_10856a88(uVar2,6);
    }
    else {
      if (param_4 == 0x28) {
        iVar1 = FUN_1084c844();
        if (iVar1 == 0) {
          return;
        }
        goto LAB_1084c51c;
      }
      iVar1 = FUN_10856a88(uVar2,param_4);
    }
    if (iVar1 == 0) {
      return;
    }
  }
LAB_1084c51c:
  iVar1 = FUN_10869f7c(param_2,10,param_3);
  *(char *)(iVar1 + 4) = (char)param_4;
  *(undefined1 *)(iVar1 + 0x11) = 1;
  *(char *)(iVar1 + 5) = (char)param_3;
  FUN_10867b48(param_2,iVar1);
  return;
}

