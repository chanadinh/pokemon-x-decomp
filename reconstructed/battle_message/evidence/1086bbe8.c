/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1086bbe8(int param_1,undefined2 *param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined2 uVar3;
  uint uVar4;
  undefined4 uVar5;
  uint uVar6;
  
  uVar4 = (uint)(ushort)param_2[1];
  uVar5 = 0xffff0000;
  uVar6 = uVar4 & 0x7f;
  if ((param_2[1] & 0x4000) == 0) {
    if (uVar6 == 1) {
      FUN_10867bbc(param_1,*param_2,(uVar4 & 0x3fff) >> 7,param_2 + 2);
      return 1;
    }
    if (uVar6 != 2) {
      return 0;
    }
    uVar4 = (uVar4 & 0x3fff) >> 7;
    uVar3 = *param_2;
    if ((((uVar4 == 1) || (uVar4 == 2)) || (uVar4 == 3)) || (uVar4 == 4)) {
      uVar5 = *(undefined4 *)(param_2 + 2);
    }
    uVar1 = *(undefined4 *)(param_1 + 0xc);
    uVar2 = 0x6a;
  }
  else {
    uVar5 = *(undefined4 *)(param_2 + 0x10);
    if (uVar6 == 1) {
      uVar4 = (uVar4 & 0x3fff) >> 7;
      uVar3 = *param_2;
      if (uVar4 == 1) {
        uVar1 = *(undefined4 *)(param_1 + 0xc);
        uVar2 = 0x6b;
      }
      else if (uVar4 == 2) {
        uVar1 = *(undefined4 *)(param_1 + 0xc);
        uVar2 = 0x6b;
      }
      else if (uVar4 == 3) {
        uVar2 = 0x6b;
        uVar1 = *(undefined4 *)(param_1 + 0xc);
      }
      else if (uVar4 == 4) {
        uVar1 = *(undefined4 *)(param_1 + 0xc);
        uVar2 = 0x6b;
      }
      else {
        uVar1 = *(undefined4 *)(param_1 + 0xc);
        uVar2 = 0x6b;
      }
    }
    else {
      if (uVar6 != 2) {
        return 0;
      }
      uVar4 = (uVar4 & 0x3fff) >> 7;
      uVar3 = *param_2;
      if (uVar4 == 1) {
        uVar1 = *(undefined4 *)(param_1 + 0xc);
        uVar2 = 0x6c;
      }
      else if (uVar4 == 2) {
        uVar2 = 0x6c;
        uVar1 = *(undefined4 *)(param_1 + 0xc);
      }
      else if (uVar4 == 3) {
        uVar2 = 0x6c;
        uVar1 = *(undefined4 *)(param_1 + 0xc);
      }
      else {
        if (uVar4 != 4) {
          FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6c,uVar3,uVar5,0xffff0000);
          return 1;
        }
        uVar1 = *(undefined4 *)(param_1 + 0xc);
        uVar2 = 0x6c;
      }
    }
  }
  FUN_1086b5e0(uVar1,uVar2,uVar3,uVar5);
  return 1;
}

