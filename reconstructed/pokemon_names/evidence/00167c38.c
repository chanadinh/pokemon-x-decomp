/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint FUN_00167c38(uint param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  iVar1 = 0;
  if (param_1 != 0x2d2) {
    iVar1 = param_1 - 0x200;
  }
  if ((param_1 == 0x2d2 || iVar1 == 0xd3) || (DAT_00167c9c < param_1)) {
    param_1 = 0;
  }
  else if (param_2 != 0) {
    iVar1 = *(int *)(DAT_00167ca0 + 4) + param_1 * 0x40;
    uVar3 = *(uint *)(iVar1 + 0x20);
    uVar2 = *(uint *)(iVar1 + 0x1c) & 0xffff;
    if (uVar2 != 0) {
      uVar3 = uVar3 & 0xff;
    }
    if (uVar2 != 0 && param_2 < uVar3) {
      return (param_2 - 1) + uVar2;
    }
  }
  return param_1;
}

