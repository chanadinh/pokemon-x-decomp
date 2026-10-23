/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_1ff520(int param_1)

{
  int *piVar1;
  int iVar2;
  
  if (param_1 == 0) {
    return;
  }
  piVar1 = (int *)anon_segment_0_2b284(*(undefined4 *)(param_1 + -0x20));
  iVar2 = (**(code **)(*piVar1 + 0x20))(piVar1);
  if (iVar2 != *(int *)(param_1 + -0x20)) {
    piVar1 = (int *)anon_segment_0_2b284();
    (**(code **)(*piVar1 + 0x5c))(piVar1,param_1 - (uint)*(ushort *)(param_1 + -0x1c));
                    /* WARNING: Could not recover jumptable at 0x002ff58c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*piVar1 + 0x4c))(piVar1);
    return;
  }
  (**(code **)(*piVar1 + 0x4c))(piVar1);
                    /* WARNING: Could not recover jumptable at 0x002ff5b8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*piVar1 + 0x5c))(piVar1,param_1 - (uint)*(ushort *)(param_1 + -0x1c));
  return;
}

