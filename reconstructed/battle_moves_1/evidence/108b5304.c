/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5304(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    uVar3 = FUN_1084c7fc();
    if (uVar3 == *(ushort *)(param_1 + 0x38)) {
      uVar3 = FUN_1086b91c(0x31);
      uVar4 = FUN_108c86c8(uVar2);
      uVar5 = 0;
      if (uVar4 != 0) {
        do {
          uVar3 = uVar3 * 2;
          if (0xa0 < uVar3) {
            uVar3 = 0xa0;
            break;
          }
          uVar5 = uVar5 + 1;
        } while (uVar5 < uVar4);
      }
      FUN_108658d4(0x31,uVar3);
      return;
    }
  }
  return;
}

