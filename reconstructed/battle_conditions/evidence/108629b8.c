/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108629b8(int param_1,int param_2,int param_3,uint param_4,undefined4 param_5,int param_6,
                 int param_7,int param_8)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int iVar4;
  undefined1 uVar5;
  undefined4 uVar6;
  bool bVar7;
  bool bVar8;
  
  FUN_10865118(param_1,param_2,param_4);
  uVar1 = param_1 + 0x120c;
  switch(param_4) {
  default:
    goto switchD_108629f4_caseD_0;
  case 1:
    uVar3 = 0x299;
    uVar1 = FUN_1086c2cc(uVar1,*(undefined1 *)(param_2 + 0x19));
    if (uVar1 == 10) {
      uVar1 = 10;
      goto switchD_108629f4_caseD_0;
    }
    break;
  case 2:
    uVar3 = 0x295;
    uVar1 = FUN_1086c2cc(uVar1,*(undefined1 *)(param_2 + 0x19));
    if (uVar1 == 10) {
      uVar1 = 10;
      goto switchD_108629f4_caseD_0;
    }
    break;
  case 3:
    uVar3 = 0x298;
    uVar1 = FUN_1086c2cc(uVar1,*(undefined1 *)(param_2 + 0x19));
    if (uVar1 == 10) goto switchD_108629f4_caseD_0;
    goto LAB_10862aa8;
  case 4:
    uVar3 = 0x297;
    uVar1 = FUN_1086c2cc(uVar1,*(undefined1 *)(param_2 + 0x19));
    if (uVar1 == 10) {
      uVar1 = 10;
      goto switchD_108629f4_caseD_0;
    }
LAB_10862aa8:
    uVar6 = *(undefined4 *)(param_1 + 0xc);
    goto LAB_10862a88;
  case 5:
    uVar1 = FUN_1086c2cc(uVar1,*(undefined1 *)(param_2 + 0x19));
    if (uVar1 != 10) {
      uVar1 = FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x57,uVar1,0x296);
    }
    goto switchD_108629f4_caseD_0;
  case 6:
    uVar3 = 0x29a;
    uVar1 = FUN_1086c2cc(uVar1,*(undefined1 *)(param_2 + 0x19));
    if (uVar1 == 10) {
      uVar1 = 10;
      goto switchD_108629f4_caseD_0;
    }
    break;
  case 7:
    uVar3 = 0x29b;
    uVar1 = FUN_1086c2cc(uVar1,*(undefined1 *)(param_2 + 0x19));
    if (uVar1 == 10) goto switchD_108629f4_caseD_0;
  }
  uVar6 = *(undefined4 *)(param_1 + 0xc);
LAB_10862a88:
  uVar1 = FUN_1086c22c(uVar6,0x57,uVar1,uVar3);
switchD_108629f4_caseD_0:
  if (param_6 == 0) {
    if (param_8 != 0) {
      uVar1 = FUN_1086bbe8(param_1,param_8);
    }
  }
  else {
    FUN_1087afa0(param_4,param_5,param_2);
    FUN_1086bbe8(param_1,(undefined4 *)(param_1 + 0x433c));
    *(undefined4 *)(param_1 + 0x433c) = 0;
    *(undefined4 *)(param_1 + 0x4340) = 0;
    *(undefined4 *)(param_1 + 0x4344) = 0;
    *(undefined4 *)(param_1 + 0x4348) = 0;
    *(undefined4 *)(param_1 + 0x434c) = 0;
    *(undefined4 *)(param_1 + 0x4350) = 0;
    *(undefined4 *)(param_1 + 0x4354) = 0;
    *(undefined4 *)(param_1 + 0x4358) = 0;
    uVar1 = *(ushort *)(param_1 + 0x433e) & 0xffffff80;
    *(undefined4 *)(param_1 + 0x435c) = 0;
    *(short *)(param_1 + 0x433e) = (short)uVar1;
  }
  bVar7 = param_4 == 3;
  if (bVar7) {
    uVar1 = (uint)*(ushort *)(param_2 + 0xc);
  }
  bVar8 = bVar7 && uVar1 == 0x1ec;
  if (bVar7 && uVar1 == 0x1ec) {
    bVar8 = *(char *)(param_2 + 0x153) == '\x01';
  }
  if (bVar8) {
    FUN_108679ac(param_2);
    FUN_1086fff0(param_2,0);
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x5b,*(undefined1 *)(param_2 + 0x19),0);
    FUN_108678f8(param_2);
    FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0xde,*(undefined1 *)(param_2 + 0x19),0xffff0000
                );
  }
  puVar2 = (undefined4 *)(param_1 + 0x437c);
  uVar3 = FUN_1086b284();
  if (param_4 < 6) {
    FUN_1086bb84();
    FUN_1086bac0(4,*(undefined1 *)(param_2 + 0x19));
    if (param_3 == 0) {
      uVar5 = 0x1f;
    }
    else {
      uVar5 = *(undefined1 *)(param_3 + 0x19);
    }
    FUN_1086bac0(3,uVar5);
    FUN_1086bac0(0x1e,param_4 & 0xff);
    FUN_1086bac0(0x1f,param_5);
    FUN_1086b9a4(param_1,0x71);
    FUN_1086b880();
  }
  else if (param_4 == 0x10) {
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(param_2 + 0x19));
    DAT_108fca68 = DAT_108fca68 + 1;
    FUN_10864e98(param_1,0x73,7,0);
    DAT_108fca68 = DAT_108fca68 + -1;
    iVar4 = DAT_108fca60;
    if (DAT_108fca68 == 0) {
      for (; iVar4 != 0; iVar4 = *(int *)(iVar4 + 4)) {
        *(undefined2 *)(iVar4 + 0x18) = 0;
      }
    }
    FUN_1086b880();
    iVar4 = FUN_10874420(param_2,0x10);
    if (iVar4 == 0x7f) {
      FUN_10864dc0(param_1,param_2);
    }
  }
  else {
    FUN_1086bb84();
    FUN_1086bac0(4,*(undefined1 *)(param_2 + 0x19));
    if (param_3 == 0) {
      uVar5 = 0x1f;
    }
    else {
      uVar5 = *(undefined1 *)(param_3 + 0x19);
    }
    FUN_1086bac0(3,uVar5);
    FUN_1086bac0(0x1e,param_4);
    FUN_1086b9a4(param_1,0x72);
    FUN_1086b880();
  }
  *puVar2 = uVar3;
  if ((param_7 == 0) && (*(short *)(param_2 + 0x12) != 0)) {
    uVar3 = FUN_1086b284(puVar2);
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(param_2 + 0x19));
    FUN_1086bac0(0x2f,4);
    FUN_1086b9a4(param_1,0x9c);
    FUN_1086b880();
    *puVar2 = uVar3;
  }
  return;
}

