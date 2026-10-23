/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined8 FUN_003e0aa8(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = param_1 >> 0x18;
  if (param_1 == 0xffffffff || uVar2 == 0xff) {
    uVar2 = 0;
    uVar1 = 0;
  }
  else {
    uVar1 = (param_1 & 0xffffff) >> 0x10;
  }
  return CONCAT44(uVar2,uVar1);
}

