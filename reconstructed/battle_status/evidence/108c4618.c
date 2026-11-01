/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4618(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  uVar2 = FUN_1086b91c(4);
  uVar2 = uVar2 & 0xff;
  iVar3 = FUN_1084c5a8(param_3,uVar2);
  if (iVar3 != 0) {
    uVar4 = FUN_1086b91c(4);
    if ((uVar4 == uVar2) && (sVar1 = FUN_1086b91c(0x1e), sVar1 == 2)) {
      iVar3 = FUN_108658d4(0x42,1);
    }
    else {
      iVar3 = 0;
    }
    *param_4 = iVar3;
    if (iVar3 == 0) {
      uVar4 = FUN_1086b91c(4);
      if ((uVar4 == uVar2) && (sVar1 = FUN_1086b91c(0x1e), sVar1 == 0xe)) {
        iVar3 = FUN_108658d4(0x42,1);
      }
      else {
        iVar3 = 0;
      }
      *param_4 = iVar3;
    }
  }
  return;
}

