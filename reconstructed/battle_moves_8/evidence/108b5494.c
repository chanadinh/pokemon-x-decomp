/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5494(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  undefined1 uVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  
  iVar5 = FUN_1086b91c(3);
  if (iVar5 == param_3) {
    uVar4 = FUN_1086b91c(4);
    uVar6 = FUN_108544a0(param_2,uVar4);
    FUN_108544a0(param_2,param_3);
    uVar7 = FUN_10870994();
    uVar8 = FUN_10870994(uVar6);
    uVar9 = uVar7 & 0x1f;
    uVar1 = (uVar7 & 0x3ff) >> 5;
    uVar7 = (uVar7 & 0x7fff) >> 10;
    uVar2 = (uVar8 & 0x3ff) >> 5;
    uVar3 = (uVar8 & 0x7fff) >> 10;
    uVar8 = uVar8 & 0x1f;
    if ((((uVar9 == 0x12) || ((uVar9 != uVar8 && uVar9 != uVar2 && (uVar9 != uVar3)))) &&
        ((uVar1 == 0x12 || ((uVar1 != uVar8 && uVar1 != uVar2 && (uVar1 != uVar3)))))) &&
       ((uVar7 == 0x12 || ((uVar7 != uVar8 && uVar7 != uVar2 && (uVar7 != uVar3)))))) {
      FUN_108658d4(0x41,1);
      return;
    }
  }
  return;
}

