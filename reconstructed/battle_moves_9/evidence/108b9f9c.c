/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b9f9c(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (*param_4 != 0)) {
    iVar1 = param_4[1];
    uVar2 = 0;
    if (iVar1 == 1) {
      uVar2 = 0x206;
    }
    else if (iVar1 == 2) {
      uVar2 = 0x207;
    }
    else if (iVar1 == 3) {
      uVar2 = 0x208;
    }
    FUN_108658d4(0x13,uVar2);
    return;
  }
  return;
}

