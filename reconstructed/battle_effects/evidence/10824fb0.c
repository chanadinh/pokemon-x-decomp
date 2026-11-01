/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10824fb0(int param_1,int param_2)

{
  byte bVar1;
  char cVar2;
  undefined1 uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  
  bVar1 = *(byte *)(param_2 + 4);
  uVar7 = *(uint *)(param_2 + 8);
  uVar4 = *(undefined4 *)(param_1 + 0x1c);
  if (bVar1 == 10) {
    uVar3 = *(undefined1 *)(param_2 + 5);
    iVar5 = FUN_1086fde0(uVar4,uVar3,uVar7);
    if (iVar5 == 0) {
      return 0;
    }
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x26,uVar3,uVar7);
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x62,0);
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x59,*(undefined1 *)(param_2 + 5));
  }
  else {
    cVar2 = *(char *)(param_2 + 0x30);
    if (10 < bVar1) {
      FUN_10800394(0,0);
    }
    if (bVar1 == 10) {
      FUN_10800394(0,0);
    }
    iVar5 = FUN_108712c0(uVar4,bVar1,uVar7,0);
    if (iVar5 == 0) {
      if (cVar2 == '\0') {
        return 0;
      }
      if ((uVar7 & 7) == 3) {
        uVar7 = uVar7 << 0x17;
      }
      else {
        if ((uVar7 & 7) != 4) {
          return 0;
        }
        uVar7 = uVar7 << 0x11;
      }
      uVar7 = uVar7 >> 0x1a;
      if (uVar7 == 0x1f) {
        return 0;
      }
      FUN_1086f878(*(undefined4 *)(param_1 + 0x1c),bVar1,uVar7);
      uVar4 = *(undefined4 *)(param_1 + 0xc);
      uVar6 = 0x25;
    }
    else {
      uVar4 = *(undefined4 *)(param_1 + 0xc);
      uVar6 = 0x24;
    }
    FUN_1086c22c(uVar4,uVar6,bVar1,uVar7);
  }
  FUN_1086bbe8(param_1,param_2 + 0xc);
  return 1;
}

