/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aadac(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  uVar1 = FUN_1086b91c(5);
  uVar4 = 0;
  if (uVar1 != 0) {
    do {
      iVar2 = FUN_1086b91c(uVar4 + 6 & 0xff);
      if (iVar2 == param_3) {
        iVar2 = FUN_1086b91c(0x1b);
        if (iVar2 != 2) {
          return;
        }
        uVar3 = FUN_108544a0(param_2,param_3);
        iVar2 = FUN_108722e8(uVar3,4,1);
        if (iVar2 == 0) {
          return;
        }
        FUN_1084c96c(param_1);
        FUN_1084c948(param_2,0,param_3);
        return;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < uVar1);
  }
  return;
}

