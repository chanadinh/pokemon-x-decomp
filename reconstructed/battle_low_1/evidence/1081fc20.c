/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081fc20(int param_1,int param_2,uint param_3,int param_4)

{
  byte bVar1;
  char cVar2;
  undefined4 uVar3;
  uint uVar4;
  
  bVar1 = *(byte *)(param_2 + 0x169 + (param_3 >> 3));
  uVar4 = 1 << (param_3 & 7);
  if ((uVar4 & 0xff & (uint)bVar1) == 0) {
    uVar3 = 0;
  }
  else {
    cVar2 = *(char *)(param_2 + 0x19);
    *(byte *)(param_2 + 0x169 + (param_3 >> 3 & 0xff)) = bVar1 & ~(byte)uVar4;
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x1c,*(undefined1 *)(param_2 + 0x19),param_3);
    if ((*(uint *)(param_2 + 0x84) & 7) != 0) {
      FUN_108695c0(param_1,param_2,0x1a,0);
    }
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x38,cVar2,0);
    if (param_4 == 0) {
      uVar4 = 0;
      if (*(byte *)(param_1 + 0x108c) != 0) {
        do {
          if (((*(uint *)(param_1 + uVar4 * 0x10 + 0x10c0) & 0xf) != 8) &&
             (*(char *)(*(int *)(param_1 + uVar4 * 0x10 + 0x10bc) + 0x19) == cVar2)) {
            param_1 = param_1 + uVar4 * 0x10;
            if (param_1 != -0x10bc) {
              *(undefined1 *)(param_1 + 0x10c9) = 1;
            }
            break;
          }
          uVar4 = uVar4 + 1;
        } while (uVar4 < *(byte *)(param_1 + 0x108c));
      }
    }
    uVar3 = 1;
  }
  return uVar3;
}

