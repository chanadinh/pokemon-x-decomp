/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b8200(undefined4 param_1,undefined4 param_2,uint param_3,uint *param_4)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  uVar1 = FUN_1086b91c(3);
  if (uVar1 != param_3) {
    return;
  }
  uVar6 = *param_4 & 0xff;
  iVar2 = FUN_1084b950(param_2,param_3);
  uVar1 = (uint)*(byte *)(iVar2 + 0x18);
  if ((uVar6 < uVar1) && (uVar5 = 0, uVar1 != 0)) {
    do {
      iVar3 = FUN_1085daf8(iVar2,uVar5 & 0xff);
      if ((*(byte *)(iVar3 + 0x19) == param_3) ||
         ((iVar4 = FUN_108551d4(iVar3), iVar4 != 0 && (iVar4 = FUN_1086d8ac(iVar3), iVar4 == 0)))) {
        if (uVar6 == 0) {
          if (iVar3 != 0) goto LAB_108b82bc;
          break;
        }
        uVar6 = uVar6 - 1 & 0xff;
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 < uVar1);
  }
  iVar3 = FUN_108544a0(param_2,param_3);
LAB_108b82bc:
  uVar1 = *param_4;
  *param_4 = uVar1 + 1;
  if (5 < uVar1 + 1) {
    *param_4 = 0;
  }
  uVar1 = FUN_10874420(iVar3,0x14);
  FUN_108658d4(0x31,uVar1 / 10 + 5);
  return;
}

