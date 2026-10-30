/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_108202d8(int param_1,int param_2,int param_3,undefined2 *param_4,int param_5,
                 undefined4 param_6)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined4 uVar6;
  
  iVar3 = thunk_FUN_0039ddc0(*param_4);
  if (iVar3 == 2) {
    uVar6 = 10;
  }
  else {
    uVar6 = 8;
  }
  FUN_1086bb84();
  FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
  FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
  FUN_1086b520(0x3c,0x1f);
  FUN_1086b520(0x53,0);
  FUN_1086b9a4(param_1,0x3e);
  uVar4 = FUN_1086b91c(0x3c);
  uVar4 = uVar4 & 0xff;
  if (uVar4 != 0x1f) {
    iVar3 = *(int *)(param_1 + 8);
    if (0x17 < uVar4) {
      FUN_10800394(0,0);
    }
    iVar3 = iVar3 + uVar4 * 4;
    if (*(int *)(iVar3 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    param_2 = *(int *)(iVar3 + 0x84);
  }
  iVar3 = FUN_1086b91c(0x53);
  if (iVar3 != 0) goto LAB_10820434;
  if (param_5 == 0) {
LAB_1082044c:
    uVar2 = FUN_10874420(param_2,uVar6);
  }
  else {
    iVar3 = FUN_108620d4(param_2,uVar6);
    if (iVar3 == 8) {
      cVar1 = *(char *)(param_2 + 0x104);
joined_r0x108203f4:
      if ('\x05' < cVar1) goto LAB_1082044c;
    }
    else {
      if (iVar3 == 9) {
        cVar1 = *(char *)(param_2 + 0x105);
      }
      else {
        if (iVar3 == 10) {
          cVar1 = *(char *)(param_2 + 0x106);
          goto joined_r0x108203f4;
        }
        if (iVar3 != 0xb) goto LAB_1082044c;
        cVar1 = *(char *)(param_2 + 0x107);
      }
      if (cVar1 < '\a') goto LAB_1082044c;
    }
LAB_10820434:
    uVar2 = FUN_108743a8(param_2,uVar6);
  }
  FUN_1086bac0(0x12,*param_4);
  FUN_1086bac0(0x16,*(undefined1 *)(param_4 + 3));
  FUN_1086bac0(0x1b,*(undefined1 *)((int)param_4 + 7));
  FUN_1086b520(0x34,uVar2);
  FUN_10867c9c(0x36,0x1000,0x19a,0x20000);
  FUN_1086b9fc(0x54,0);
  FUN_1086b9a4(param_1,0x40);
  uVar4 = FUN_1086b91c(0x34);
  iVar3 = FUN_1086b91c(0x36);
  uVar5 = (uVar4 & 0xffff) * iVar3;
  uVar4 = uVar5 >> 0xc;
  if (0x800 < (uVar5 & 0xfff)) {
    uVar4 = uVar4 + 1;
  }
  iVar3 = FUN_1086b91c(0x54);
  *(bool *)param_6 = iVar3 != 0;
  FUN_1086b880();
  return uVar4 & 0xffff;
}

