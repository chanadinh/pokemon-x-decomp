/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4
FUN_1086a1e0(int param_1,int param_2,int param_3,undefined2 *param_4,undefined4 param_5,int param_6,
            int param_7,int param_8,undefined4 param_9,undefined2 *param_10)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  char cVar8;
  uint uVar9;
  bool bVar10;
  undefined4 local_2c;
  byte local_28 [4];
  
  local_2c = 0;
  iVar2 = thunk_FUN_0039ddc0(*param_4);
  FUN_1086bb84();
  FUN_1086bac0(0x39,param_5);
  FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
  FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
  FUN_1086bac0(0x46,param_7);
  FUN_1086bac0(0x16,*(undefined1 *)(param_4 + 3));
  FUN_1086bac0(0x12,*param_4);
  FUN_1086bac0(0x1b,iVar2);
  FUN_1086bac0(0x47,param_9);
  FUN_1086b520(0x38,0);
  FUN_1086b9a4(param_1,0x4e);
  uVar3 = FUN_1086b91c(0x38);
  if (uVar3 != 0) {
    local_2c = 1;
    goto LAB_1086a6a8;
  }
  uVar4 = thunk_FUN_0039e044(*param_4);
  FUN_1086bb84();
  FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
  FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
  FUN_1086bac0(0x12,*param_4);
  FUN_1086bac0(0x16,*(undefined1 *)(param_4 + 3));
  FUN_1086bac0(0x1b,*(undefined1 *)((int)param_4 + 7));
  FUN_1086b9fc(0x57,0);
  FUN_1086b9fc(0x58,0);
  FUN_1086b9fc(0x31,uVar4);
  FUN_1086b9a4(param_1,0x3c);
  FUN_10867c9c(0x32,0x1000,0x29,0x200000);
  FUN_1086b9a4(param_1,0x3d);
  uVar3 = FUN_1086b91c(0x31);
  iVar5 = FUN_1086b91c(0x32);
  FUN_1086b880();
  uVar6 = (uVar3 & 0xffff) * iVar5;
  uVar3 = uVar6 >> 0xc;
  if (0x800 < (uVar6 & 0xfff)) {
    uVar3 = uVar3 + 1;
  }
  iVar5 = FUN_108202d8(param_1,param_2,param_3,param_4,param_7,local_28);
  uVar4 = FUN_10822330(param_1,param_2,param_3,param_4,param_7);
  uVar6 = FUN_10874420(param_2,0xf);
  uVar3 = thunk_FUN_0015e958((((uVar6 & 0xff) * 2) / 5 + 2) * (uVar3 & 0xffff) * iVar5,uVar4,
                             (uVar6 & 0xff) * -0x66666666);
  uVar6 = uVar3 * 0x51eb851f;
  uVar3 = uVar3 / 0x32;
  uVar9 = uVar3 + 2;
  if (param_6 != 0x1000) {
    uVar3 = uVar9 * param_6;
    uVar9 = uVar3 >> 0xc;
    uVar6 = uVar3 & 0xfff;
    if (0x800 < uVar6) {
      uVar9 = uVar9 + 1;
    }
  }
  FUN_1086bb84(uVar3,uVar6);
  FUN_1086b9fc(0x42,0);
  FUN_1086b9a4(param_1,0x83);
  iVar5 = FUN_1086b91c(0x42);
  if (iVar5 == 0) {
    FUN_1086b880();
    cVar8 = *(char *)(*(int *)(param_1 + 0x1c) + 1);
  }
  else {
    FUN_1086b880();
    cVar8 = '\0';
  }
  cVar1 = *(char *)(param_4 + 3);
  if (cVar8 == '\x01') {
    if (cVar1 == '\t') {
LAB_1086a4ac:
      iVar5 = 0x1800;
LAB_1086a4cc:
      uVar3 = uVar9 * iVar5;
      uVar9 = uVar3 >> 0xc;
      if (0x800 < (uVar3 & 0xfff)) {
        uVar9 = uVar9 + 1;
      }
    }
    else if (cVar1 == '\n') goto LAB_1086a4c8;
  }
  else if (cVar8 == '\x02') {
    if (cVar1 == '\t') {
LAB_1086a4c8:
      iVar5 = 0x800;
      goto LAB_1086a4cc;
    }
    if (cVar1 == '\n') goto LAB_1086a4ac;
  }
  if (param_7 != 0) {
    uVar9 = (uVar9 * 0xf) / 10;
  }
  iVar5 = thunk_FUN_001ce05c(*(undefined4 *)(*(int *)(param_1 + 4) + 0x10),7);
  if (iVar5 == 0) {
    if (param_8 == 0) {
      iVar5 = thunk_FUN_00174bb4(DAT_108fca54,0x10);
      uVar3 = 100U - iVar5 & 0xffff;
    }
    else {
      uVar3 = 0x55;
    }
    uVar9 = (uVar9 * uVar3) / 100;
  }
  if (*(char *)(param_4 + 3) != '\x12') {
    uVar7 = FUN_1086f960(param_2);
    uVar4 = 0x1000;
    FUN_1086bb84();
    FUN_1086bac0(2,*(undefined1 *)(param_2 + 0x19));
    FUN_1086b9fc(0x53,uVar7);
    FUN_1086b9a4(param_1,0x48);
    iVar5 = FUN_1086b91c(0x53);
    if (iVar5 != 0) {
      uVar4 = 0x1800;
    }
    FUN_1086bac0(0x45,iVar5 != 0);
    FUN_1086b520(0x36,uVar4);
    FUN_1086b9a4(param_1,0x49);
    iVar5 = FUN_1086b91c(0x36);
    FUN_1086b880();
    uVar3 = uVar9 * iVar5;
    uVar9 = uVar3 >> 0xc;
    if (0x800 < (uVar3 & 0xfff)) {
      uVar9 = uVar9 + 1;
    }
  }
  uVar3 = FUN_1089e7f0(uVar9,param_5);
  if (iVar2 == 1) {
    uVar6 = FUN_1086d8ac(param_2);
    bVar10 = uVar6 == 4;
    if (bVar10) {
      uVar6 = (uint)local_28[0];
    }
    if (bVar10 && uVar6 == 0) {
      uVar3 = (uVar3 * 0x32) / 100;
    }
  }
  FUN_10867c9c(0x36,0x1000,0x29,0x20000);
  FUN_1086b520(0x33,uVar3);
  FUN_1086b9a4(param_1,0x4f);
  iVar2 = FUN_1086b91c(0x36);
  iVar5 = FUN_1086b91c(0x33);
  uVar3 = (uint)(iVar5 * iVar2) >> 0xc;
  if (0x800 < (iVar5 * iVar2 & 0xfffU)) {
    uVar3 = uVar3 + 1;
  }
  if (uVar3 == 0) {
    uVar3 = 1;
  }
LAB_1086a6a8:
  FUN_1086b9a4(param_1,0x50);
  FUN_1086b880();
  *param_10 = (short)uVar3;
  return local_2c;
}

