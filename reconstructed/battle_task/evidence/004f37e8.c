/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 * FUN_004f37e8(undefined4 param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  uVar1 = _static_::anon_segment_0_2b27c();
  iVar2 = _static_::anon_segment_0_22920(0x26c,uVar1);
  puVar3 = (undefined4 *)0x0;
  if (iVar2 != 0) {
    uVar1 = _static_::anon_segment_0_2b27c(param_1);
    puVar3 = (undefined4 *)_static_::anon_segment_0_9498(iVar2,uVar1);
    uVar1 = DAT_004f3874;
    puVar3[6] = 0;
    puVar3[7] = 0;
    puVar3[8] = 0;
    *puVar3 = uVar1;
    *(undefined1 *)(puVar3 + 10) = 0;
    *(undefined1 *)((int)puVar3 + 0x29) = 0;
    *(undefined1 *)((int)puVar3 + 0x2a) = 0;
    *(undefined1 *)((int)puVar3 + 0x2b) = 0;
    puVar3[0x96] = 0;
    *(undefined1 *)((int)puVar3 + 0x262) = 0;
    puVar3[0x99] = 0;
    puVar3[0x9a] = 0xffffffff;
  }
  _static_::anon_segment_0_8cf0(param_1,puVar3);
  return puVar3;
}

