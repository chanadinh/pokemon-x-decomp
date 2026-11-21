/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_108676cc(undefined4 param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined8 uVar5;
  
  uVar1 = FUN_10874420(param_2,0xc);
  FUN_1086bb84();
  FUN_1086bac0(2,*(undefined1 *)(param_2 + 0x19));
  FUN_1086bac0(0x30,uVar1);
  FUN_1086b520(0x53,1);
  FUN_1086b520(0x4b,0);
  FUN_10867c9c(0x36,0x1000,0x19a,0x20000);
  FUN_1086b9a4(param_1,0x13);
  iVar2 = FUN_1086b91c(0x30);
  iVar3 = FUN_1086b91c(0x36);
  uVar4 = (uint)(iVar2 * iVar3) >> 0xc;
  if (0x800 < (iVar2 * iVar3 & 0xfffU)) {
    uVar4 = uVar4 + 1;
  }
  uVar5 = FUN_1086d8ac(param_2);
  iVar2 = (int)((ulonglong)uVar5 >> 0x20);
  if ((int)uVar5 == 1) {
    uVar5 = FUN_1086b91c(0x53);
    iVar2 = (int)((ulonglong)uVar5 >> 0x20);
    if ((int)uVar5 != 0) {
      iVar2 = uVar4 * 7;
      uVar4 = (uVar4 * 0x19) / 100;
    }
  }
  if (10000 < uVar4) {
    uVar4 = 10000;
  }
  if (param_3 != 0) {
    iVar2 = FUN_1086b91c(0x4b,iVar2);
    if (iVar2 != 0) {
      uVar4 = 10000 - uVar4;
    }
  }
  FUN_1086b880();
  return uVar4 & 0xffff;
}

