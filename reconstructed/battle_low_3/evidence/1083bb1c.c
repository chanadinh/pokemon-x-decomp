/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083bb1c(int *param_1,int param_2,undefined1 param_3,byte param_4,int param_5)

{
  byte bVar1;
  undefined1 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined1 *puVar5;
  uint uVar6;
  undefined1 auStack_31 [13];
  
  if (DAT_108fcb84 != 0x993) {
    DAT_108fcb84 = 0x993;
  }
  param_1[0x1d8] = param_2;
  param_1[0x1d9] = param_5;
  iVar3 = (**(code **)(*param_1 + 0x24))(param_1);
  bVar1 = *(byte *)(iVar3 + 0x18);
  uVar6 = 0;
  if (bVar1 != 0) {
    do {
      uVar4 = FUN_1085daf8(iVar3,uVar6 & 0xff);
      uVar2 = FUN_10856a88(uVar4,0x13);
      auStack_31[uVar6 + 1] = uVar2;
      uVar6 = uVar6 + 1;
    } while (uVar6 < bVar1);
    if ((int)(6 - uVar6) < 1) goto LAB_1083bbdc;
  }
  puVar5 = auStack_31 + uVar6;
  if ((6 - uVar6 & 1) != 0) {
    auStack_31[uVar6 + 1] = 0;
    puVar5 = auStack_31 + uVar6 + 1;
  }
  for (iVar3 = (int)(6 - uVar6) >> 1; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar5[1] = 0;
    puVar5 = puVar5 + 2;
    *puVar5 = 0;
  }
LAB_1083bbdc:
  iVar3 = param_1[100];
  *(undefined4 *)(iVar3 + 0x3c) = 0;
  *(byte *)(iVar3 + 0x4a) = param_4 ^ 1;
  *(byte *)(iVar3 + 0x138) = param_4 ^ 1;
  *(undefined1 *)(iVar3 + 0x13f) = param_3;
  *(undefined1 *)(iVar3 + 0x40) = param_3;
  FUN_1085fa8c(iVar3 + 0x139,auStack_31 + 1);
  return;
}

