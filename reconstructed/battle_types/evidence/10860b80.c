/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

uint FUN_10860b80(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  
  if (0xd < param_1) {
    FUN_10800394(0,0);
  }
  if (0xd < param_2) {
    FUN_10800394(0,0);
  }
  uVar2 = (uint)(*(int *)(&DAT_108d2248 + param_1 * 4) * *(int *)(&DAT_108d2248 + param_2 * 4)) >> 6
  ;
  if (uVar2 < 0x1001) {
    if (((uVar2 == 0) && (param_1 != 0)) && (param_2 != 0)) {
      FUN_10800394(0,0);
      uVar2 = 1;
    }
  }
  else {
    FUN_10800394(0,0);
    uVar2 = 0x1000;
  }
  uVar1 = 0;
  do {
    if ((uVar2 & 1 << (uVar1 & 0xff)) != 0) {
      uVar1 = uVar1 + 1;
LAB_10860c58:
      if (0xd < uVar1) {
        FUN_10800394(0,0);
        uVar1 = 0xd;
      }
      goto LAB_10860c74;
    }
    uVar3 = uVar1 + 1;
    uVar1 = uVar1 + 2;
    if ((uVar2 & 1 << (uVar3 & 0xff)) != 0) goto LAB_10860c58;
  } while (uVar1 < 0x20);
  uVar1 = 0;
LAB_10860c74:
  return uVar1 & 0xff;
}

