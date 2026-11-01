/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108266b8(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  
  uVar4 = 0;
  uVar3 = 0;
  do {
    uVar2 = (uVar3 >> 3) + 1 & 0xff;
    if ((uVar2 < *(byte *)(param_2 + 4)) &&
       (((uint)*(byte *)(param_2 + 4 + uVar2) & 1 << (uVar3 & 7)) != 0)) {
      iVar1 = FUN_108ace94(*(undefined1 *)(param_2 + 8),uVar3 & 0xff,uVar3 & 7,
                           (uint)*(byte *)(param_2 + 4),param_4);
      if (iVar1 == 0) goto LAB_108267e8;
      switch(uVar3 & 0xff) {
      case 0:
        uVar4 = 0x84;
        break;
      case 1:
        uVar4 = 0x88;
        break;
      case 2:
        uVar4 = 0x8c;
        break;
      case 3:
        uVar4 = 0x90;
        break;
      case 4:
        uVar4 = 0x94;
        break;
      case 5:
        uVar4 = 0x98;
        break;
      case 6:
        uVar4 = 0x9c;
        break;
      case 7:
        uVar4 = 0xa0;
        break;
      case 8:
        uVar4 = 0xa4;
        break;
      default:
        goto LAB_108267e4;
      case 0xb:
        uVar4 = 0xac;
        break;
      case 0xc:
        uVar4 = 0xb0;
        break;
      case 0xd:
        uVar4 = 0xb4;
        break;
      case 0xe:
        uVar4 = 0xd8;
      }
      param_4 = 0xffff0000;
      FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x69,uVar4);
LAB_108267e4:
      uVar4 = 1;
    }
LAB_108267e8:
    uVar3 = uVar3 + 1;
    if (0x10 < uVar3) {
      return uVar4;
    }
  } while( true );
}

