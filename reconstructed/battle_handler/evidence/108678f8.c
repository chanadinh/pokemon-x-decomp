/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108678f8(int param_1)

{
  undefined1 uVar1;
  ushort uVar2;
  undefined2 uVar3;
  undefined4 uVar4;
  uint uVar5;
  uint local_1c;
  
  uVar2 = FUN_10874420(param_1,0x10);
  uVar5 = 0;
  do {
    if ((byte)(&DAT_108d510c)[uVar5 * 8] == uVar2) {
      uVar3 = FUN_108743a8(param_1,0xc);
      uVar1 = *(undefined1 *)(param_1 + 0x19);
      uVar4 = (*(code *)(&DAT_108d5110)[uVar5 * 2])(&local_1c);
      uVar5 = local_1c >> 0x10;
      local_1c = local_1c & 0xffff;
      if (uVar5 == 0) {
        uVar5 = 5;
      }
      uVar4 = FUN_1084cfc0(4,uVar2,uVar5 & 0xff,uVar3,uVar1,uVar4,local_1c);
      return uVar4;
    }
    uVar5 = uVar5 + 1;
  } while (uVar5 < 0xb6);
  return 0;
}

