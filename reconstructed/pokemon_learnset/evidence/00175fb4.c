/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00175fb4(undefined2 *param_1,undefined2 *param_2)

{
  _static_::anon_segment_0_200ef8(*(undefined4 *)(param_2 + 2),*(undefined4 *)(param_1 + 2),0x68);
  *param_2 = *param_1;
  *(undefined1 *)(param_2 + 1) = *(undefined1 *)(param_1 + 1);
  *(undefined1 *)(param_2 + 4) = *(undefined1 *)(param_1 + 4);
  return;
}

