/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_10862858(int param_1,int param_2,int param_3,undefined4 param_4,undefined4 param_5,
                 undefined4 param_6,char param_7)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined1 uVar4;
  bool bVar5;
  
  iVar1 = FUN_1082008c(param_1,param_2,param_4,param_5,param_6);
  if (iVar1 == 0) {
    uVar2 = FUN_1086b284();
    FUN_1086bb84();
    if (param_3 == 0) {
      uVar4 = 0x1f;
    }
    else {
      uVar4 = *(undefined1 *)(param_3 + 0x19);
    }
    FUN_1086bac0(3,uVar4);
    FUN_1086bac0(4,*(undefined1 *)(param_2 + 0x19));
    FUN_1086bac0(0x1e,param_4);
    FUN_1086b9fc(0x42,0);
    FUN_1086b9fc(0x44,(int)param_7);
    FUN_1086b9a4(param_1,0x6e);
    iVar1 = FUN_1086b91c(0x42);
    bVar5 = iVar1 != 0;
    iVar1 = FUN_1086b91c(0x44);
    uVar3 = FUN_1086b880();
    if (bVar5) {
      uVar3 = (uint)(iVar1 != 0);
    }
    if (bVar5 && uVar3 != 0) {
      FUN_1086bb84();
      FUN_1086b520(4,*(undefined1 *)(param_2 + 0x19));
      FUN_1086b520(3,*(undefined1 *)(param_3 + 0x19));
      FUN_1086b520(0x1e,param_4);
      FUN_1086b9a4(param_1,0x70);
      FUN_1086b880();
      *(byte *)(param_1 + 0x1093) = *(byte *)(param_1 + 0x1093) | 0x10;
    }
    *(undefined4 *)(param_1 + 0x437c) = uVar2;
    return bVar5;
  }
  if (param_7 != '\0') {
    FUN_1081cd58(param_1,param_2,iVar1,param_4);
  }
  return true;
}

