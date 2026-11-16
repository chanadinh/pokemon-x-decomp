/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ab734(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  uVar1 = FUN_1086b91c(5);
  uVar3 = 0;
  if (uVar1 != 0) {
    do {
      iVar2 = FUN_1086b91c(uVar3 + 6 & 0xff);
      if (iVar2 == param_3) {
        iVar2 = FUN_1086b91c(0x4e);
        if (iVar2 != 0) {
          return;
        }
        iVar2 = FUN_1084c030(param_2,param_3);
        if (iVar2 != 0) {
          return;
        }
        iVar2 = FUN_10865c48(param_2);
        if (iVar2 != 0) {
          return;
        }
        iVar2 = FUN_1084c020(param_2);
        if (iVar2 != 0) {
          return;
        }
        iVar2 = FUN_1084ca78(param_2,param_3);
        if (iVar2 == 0) {
          return;
        }
        iVar2 = FUN_1084c000(param_2);
        if (iVar2 == 0) {
          return;
        }
        FUN_1084c96c(param_1);
        FUN_1084c948(param_2,0,param_3);
        return;
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < uVar1);
  }
  return;
}

