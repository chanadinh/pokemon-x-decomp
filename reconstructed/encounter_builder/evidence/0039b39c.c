/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0039b39c(int param_1,int param_2)

{
  char cVar1;
  int extraout_r1;
  int iVar2;
  int extraout_r1_00;
  int extraout_r1_01;
  
  cVar1 = *(char *)(param_1 + 0xd);
  iVar2 = param_2;
  if (cVar1 != '\0') {
    *(undefined1 *)(param_1 + 0xd) = 0;
    FUN_001685f0();
    if ((*(char *)(param_1 + 0xd) != '\0') ||
       (iVar2 = extraout_r1, *(char *)(param_1 + 0xc) == '\0')) {
      iVar2 = 0;
    }
  }
  if (param_2 != 0) {
    _static_::anon_segment_0_200ef8(param_2,*(undefined4 *)(param_1 + 8),0xe8);
    iVar2 = extraout_r1_00;
  }
  if (param_2 + 0xe8 != 0) {
    _static_::anon_segment_0_200ef8(param_2 + 0xe8,*(undefined4 *)(param_1 + 4),0x1c);
    iVar2 = extraout_r1_01;
  }
  if (cVar1 != '\0') {
    FUN_00168660(param_1,iVar2);
    *(undefined1 *)(param_1 + 0xd) = 1;
    if (*(char *)(param_1 + 0xc) != '\0') {
      _static_::anon_segment_0_4f220(0,0);
      return;
    }
  }
  return;
}

