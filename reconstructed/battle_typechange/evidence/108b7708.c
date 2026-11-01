/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b7708(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (iVar1 = FUN_1084b5b8(param_2,param_3), iVar1 != 0)) {
    iVar1 = FUN_108544a0(param_2,param_3);
    uVar2 = (uint)*(ushort *)(iVar1 + 0x12);
    iVar1 = uVar2 - 0x132;
    uVar3 = 0;
    if (uVar2 == 0x132) {
      uVar3 = 2;
    }
    else if (uVar2 < 0x133) {
      switch(uVar2) {
      case 0x12a:
        uVar3 = 9;
        break;
      case 299:
        uVar3 = 10;
        break;
      case 300:
        uVar3 = 0xc;
        break;
      case 0x12d:
        uVar3 = 0xb;
        break;
      case 0x12e:
        uVar3 = 0xe;
        break;
      case 0x12f:
        uVar3 = 1;
        break;
      case 0x130:
        uVar3 = 3;
        break;
      case 0x131:
        uVar3 = 4;
      }
    }
    else if (iVar1 == 5) {
      uVar3 = 0xf;
    }
    else if (iVar1 < 6) {
      if (iVar1 == 1) {
        uVar3 = 0xd;
      }
      else if (iVar1 == 2) {
        uVar3 = 6;
      }
      else if (iVar1 == 3) {
        uVar3 = 5;
      }
      else if (iVar1 == 4) {
        uVar3 = 7;
      }
    }
    else if (iVar1 == 6) {
      uVar3 = 0x10;
    }
    else if (iVar1 == 7) {
      uVar3 = 8;
    }
    else if (iVar1 == 0x152) {
      uVar3 = 0x11;
    }
    FUN_108658d4(0x16,uVar3);
    return;
  }
  return;
}

