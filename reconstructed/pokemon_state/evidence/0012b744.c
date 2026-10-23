/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0012b744(int param_1)

{
  char cVar1;
  
  if (*(int *)(param_1 + 4) != 0) {
    cVar1 = *(char *)(param_1 + 0xd);
    if (cVar1 == '\0') {
      FUN_00168660(param_1);
      *(undefined1 *)(param_1 + 0xd) = 1;
    }
    _static_::anon_segment_0_5e914(*(undefined4 *)(param_1 + 4),0x1c);
    if (cVar1 == '\0') {
      *(undefined1 *)(param_1 + 0xd) = 0;
      FUN_001685f0(param_1);
      if ((*(char *)(param_1 + 0xd) != '\0') || (*(char *)(param_1 + 0xc) == '\0')) {
        _static_::anon_segment_0_4f220(0,0);
        return;
      }
    }
  }
  return;
}

