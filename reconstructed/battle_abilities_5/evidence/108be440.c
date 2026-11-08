/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108be440(undefined4 param_1,undefined4 param_2,uint param_3,undefined4 *param_4)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  
  uVar2 = FUN_1086b91c(3);
  if (((uVar2 & 0xff) != param_3) && (iVar3 = FUN_1086b91c(0x16), iVar3 == 0xc)) {
    sVar1 = FUN_1086b91c(0x12);
    iVar3 = FUN_1084b67c(param_2,uVar2 & 0xff,param_3,sVar1);
    if ((iVar3 == 0) && ((iVar3 = FUN_1084c720(sVar1), iVar3 == 0 && (sVar1 != 0x1fb)))) {
      uVar2 = FUN_1086b91c(4);
      iVar3 = FUN_108658d4(4,param_3);
      if (iVar3 != 0 && (uVar2 & 0xff) != param_3) {
        uVar4 = 1;
        goto LAB_108be4e4;
      }
    }
  }
  uVar4 = 0;
LAB_108be4e4:
  *param_4 = uVar4;
  return;
}

