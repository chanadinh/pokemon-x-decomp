/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0039b684(int param_1,int param_2)

{
  if (param_2 != 0) {
    if (*(int *)(param_1 + 8) == 0) {
      _static_::anon_segment_0_1e864(0,0);
    }
    _static_::anon_segment_0_200ef8(*(undefined4 *)(param_1 + 8),param_2,0xe8);
  }
  if (param_2 + 0xe8 != 0) {
    if (*(int *)(param_1 + 4) == 0) {
      _static_::anon_segment_0_1e864(0,0);
    }
    _static_::anon_segment_0_200ef8(*(undefined4 *)(param_1 + 4),param_2 + 0xe8,0x1c);
  }
  *(undefined1 *)(param_1 + 0xd) = 0;
  *(undefined1 *)(param_1 + 0xc) = 1;
  FUN_00168660(param_1);
  FUN_001685f0(param_1);
  return;
}

