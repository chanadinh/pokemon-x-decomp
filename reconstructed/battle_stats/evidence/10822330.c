/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_10822330(int param_1,int param_2,int param_3,undefined2 *param_4,int param_5)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined1 uVar6;
  
  iVar2 = thunk_FUN_0039ddc0(*param_4);
  uVar6 = *(undefined1 *)((int)param_4 + 7);
  if (iVar2 == 2) {
    iVar2 = 0xb;
  }
  else {
    iVar2 = 9;
  }
  FUN_1086bb84();
  FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
  FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
  FUN_1086bac0(0x3d,iVar2);
  FUN_1086b520(0x3e,0);
  FUN_1086b9fc(0x53,0);
  FUN_1086b9a4(param_1,0x3f);
  uVar3 = FUN_1086b91c(0x3e);
  if ((uVar3 & 1) != 0) {
    if (iVar2 == 9) {
      iVar2 = 0xb;
      uVar6 = 2;
    }
    else {
      iVar2 = 9;
      uVar6 = 1;
    }
  }
  cVar1 = FUN_1086b91c(0x53);
  FUN_1086b880();
  if (cVar1 != '\0') goto LAB_1082246c;
  if (param_5 == 0) {
LAB_10822484:
    uVar3 = FUN_10874420(param_3,iVar2);
  }
  else {
    iVar4 = FUN_108620d4(param_3,iVar2);
    if (iVar4 == 8) {
      cVar1 = *(char *)(param_3 + 0x104);
joined_r0x10822438:
      if ('\x05' < cVar1) goto LAB_10822484;
    }
    else {
      if (iVar4 == 9) {
        cVar1 = *(char *)(param_3 + 0x105);
      }
      else {
        if (iVar4 == 10) {
          cVar1 = *(char *)(param_3 + 0x106);
          goto joined_r0x10822438;
        }
        if (iVar4 != 0xb) goto LAB_10822484;
        cVar1 = *(char *)(param_3 + 0x107);
      }
      if (cVar1 < '\a') goto LAB_10822484;
    }
LAB_1082246c:
    uVar3 = FUN_108743a8(param_3,iVar2);
  }
  uVar5 = uVar3 & 0xffff;
  FUN_1086bb84();
  FUN_1086b9fc(0x42,0);
  FUN_1086b9a4(param_1,0x83);
  iVar4 = FUN_1086b91c(0x42);
  if (iVar4 == 0) {
    FUN_1086b880();
    if (((*(char *)(*(int *)(param_1 + 0x1c) + 1) == '\x04') &&
        (iVar4 = FUN_1086f960(param_3,5), iVar4 != 0)) && (iVar2 == 0xb)) {
      uVar5 = uVar5 * 0x1800 >> 0xc;
      if (0x800 < (uVar3 & 1) * 0x800) {
        uVar5 = uVar5 + 1;
      }
      uVar5 = uVar5 & 0xffff;
    }
  }
  else {
    FUN_1086b880();
  }
  FUN_1086bb84();
  FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
  FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
  FUN_1086bac0(0x12,*param_4);
  FUN_1086bac0(0x16,*(undefined1 *)(param_4 + 3));
  FUN_1086bac0(0x1b,uVar6);
  FUN_1086b520(0x35,uVar5);
  FUN_10867c9c(0x36,0x1000,0x19a,0x20000);
  FUN_1086b9a4(param_1,0x41);
  uVar3 = FUN_1086b91c(0x35);
  iVar2 = FUN_1086b91c(0x36);
  FUN_1086b880();
  uVar5 = (uVar3 & 0xffff) * iVar2;
  uVar3 = uVar5 >> 0xc;
  if (0x800 < (uVar5 & 0xfff)) {
    uVar3 = uVar3 + 1;
  }
  return uVar3 & 0xffff;
}

