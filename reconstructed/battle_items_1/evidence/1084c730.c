/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1084c730(undefined4 param_1,undefined4 param_2,uint param_3)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  uVar1 = FUN_108544a0();
  iVar2 = FUN_10874420(uVar1,0x11);
  if ((iVar2 == 0x52) && (2 < param_3)) {
    param_3 = param_3 >> 1;
  }
  if (param_3 == 0) {
    FUN_10800394(0,0);
    param_3 = 1;
  }
  uVar3 = FUN_10874420(uVar1,0xe);
  if (1 < uVar3) {
    uVar3 = FUN_10874420(uVar1,0xd);
    uVar4 = FUN_1084c124(uVar1,param_3);
    if (uVar3 <= uVar4) {
      return 1;
    }
  }
  return 0;
}

