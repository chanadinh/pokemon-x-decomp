/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108bc690(uint param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 uVar2;
  uint local_14 [2];
  
  uVar1 = 0;
  do {
    if ((byte)(&DAT_108d6768)[uVar1 * 8] == param_1) {
      uVar2 = (*(code *)(&DAT_108d676c)[uVar1 * 2])(local_14,param_2);
      uVar2 = FUN_1084cfc0(3,param_1,3,0,param_2,uVar2,local_14[0] & 0xffff);
      return uVar2;
    }
    uVar1 = uVar1 + 1;
  } while (uVar1 < 0xb);
  return 0;
}

