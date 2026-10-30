/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_1086094c(undefined4 param_1,int param_2,int param_3,undefined4 param_4,undefined4 param_5)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  
  FUN_1086bb84();
  FUN_1086bac0(3,*(undefined1 *)(param_2 + 0x19));
  FUN_1086bac0(4,*(undefined1 *)(param_3 + 0x19));
  FUN_1086bac0(0x15,param_5);
  FUN_1086bac0(0x16,param_4);
  FUN_1086b9fc(0x4c,0);
  FUN_1086b9fc(0x4d,0);
  FUN_1086b9fc(0x39,0xe);
  FUN_1086b9fc(0x17,0x12);
  FUN_10888a88(param_3);
  FUN_1086b9a4(param_1,0x44);
  iVar2 = FUN_1086b91c(0x4c);
  iVar3 = FUN_1086b91c(0x4d);
  cVar1 = FUN_1086b91c(0x17);
  uVar4 = FUN_1086b91c(0x39);
  uVar4 = uVar4 & 0xff;
  FUN_1086b880();
  if (iVar3 == 0) {
    if (uVar4 == 0xe) {
      uVar4 = FUN_10860c80(param_4,param_5);
    }
    if (cVar1 != '\x12') {
      uVar5 = FUN_10860c80(cVar1,param_5);
      uVar4 = FUN_10860b80(uVar4,uVar5);
    }
    if ((uVar4 != 0) || (iVar2 == 0)) {
      return uVar4;
    }
  }
  return 7;
}

