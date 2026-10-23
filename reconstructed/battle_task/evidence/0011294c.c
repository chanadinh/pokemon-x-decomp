/* Unreviewed Ghidra pseudocode; types require assembly review. */

int _static_::anon_segment_0_1294c(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  *(undefined1 *)(param_1 + 0x19) = 0;
  uVar3 = 0;
  *(undefined1 *)(param_1 + 0x18) = 0;
  do {
    iVar1 = anon_segment_0_22920(0x10,param_2);
    uVar2 = 0;
    if (iVar1 != 0) {
      uVar2 = anon_segment_0_1ed50(iVar1,param_2,0,1,0,0);
    }
    *(undefined4 *)(param_1 + uVar3 * 4) = uVar2;
    uVar3 = uVar3 + 1;
  } while (uVar3 < 6);
  return param_1;
}

