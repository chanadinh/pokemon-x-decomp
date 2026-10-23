/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 * _static_::anon_segment_0_29a5a8(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = (undefined4 *)anon_segment_0_29daf4();
  *puVar1 = DAT_0039a624;
  uVar2 = anon_segment_0_40120(puVar1);
  uVar2 = FUN_0012b6d0(param_2,uVar2);
  puVar1[2] = uVar2;
  FUN_0013a7a0(puVar1[3],puVar1[1],uVar2);
  uVar2 = anon_segment_0_2f9a4(puVar1);
  FUN_0012b744(puVar1[3]);
  FUN_0012b808(puVar1,1);
  anon_segment_0_3a7bc(puVar1);
  anon_segment_0_2f978(puVar1,uVar2);
  return puVar1;
}

