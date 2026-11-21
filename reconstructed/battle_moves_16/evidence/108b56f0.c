/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b56f0(undefined4 param_1,undefined4 param_2,uint param_3,undefined4 *param_4)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  
  uVar1 = FUN_1086b91c(3);
  if (uVar1 == param_3) {
    uVar1 = 0;
    iVar8 = 0;
    do {
      uVar7 = uVar1 & 0xff;
      iVar2 = FUN_1084b950(param_2,param_3);
      uVar3 = (uint)*(byte *)(iVar2 + 0x18);
      if ((uVar7 < uVar3) && (uVar6 = 0, uVar3 != 0)) {
        do {
          iVar4 = FUN_1085daf8(iVar2,uVar6 & 0xff);
          if ((*(byte *)(iVar4 + 0x19) == param_3) ||
             ((iVar5 = FUN_108551d4(iVar4), iVar5 != 0 && (iVar5 = FUN_1086d8ac(iVar4), iVar5 == 0))
             )) {
            if (uVar7 == 0) {
              if (iVar4 != 0) {
                iVar8 = iVar8 + 1;
              }
              break;
            }
            uVar7 = uVar7 - 1 & 0xff;
          }
          uVar6 = uVar6 + 1;
        } while (uVar6 < uVar3);
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 < 6);
    if (iVar8 == 0) {
      iVar8 = 1;
    }
    FUN_108658d4(0x2b,iVar8);
    *param_4 = 0;
  }
  return;
}

