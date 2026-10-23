/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 * _static_::anon_segment_0_29dc54(undefined4 *param_1)

{
  *param_1 = DAT_0039dcb4;
  if (param_1[1] != 0) {
    anon_segment_0_1ff520();
    param_1[1] = 0;
  }
  if (param_1[2] != 0) {
    anon_segment_0_1ff520();
    param_1[2] = 0;
  }
  if ((int *)param_1[3] != (int *)0x0) {
    (**(code **)(*(int *)param_1[3] + 4))();
    param_1[3] = 0;
  }
  return param_1;
}

