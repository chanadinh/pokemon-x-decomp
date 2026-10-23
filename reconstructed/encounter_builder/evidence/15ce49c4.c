/* Unreviewed Ghidra pseudocode; types require assembly review. */

char DllField::anon_segment_0_e4844(int param_1)

{
  char cVar1;
  int iVar2;
  
  cVar1 = *(char *)(param_1 + 0x8e);
  if (cVar1 == '@') {
    iVar2 = *(int *)(param_1 + 0x88);
    if (iVar2 == 0) {
      _static_::anon_segment_0_4f220(0,0);
      return '\0';
    }
    if (*(char *)(param_1 + 0x84) == '\n') {
      cVar1 = *(char *)(iVar2 + 5);
    }
    else {
      cVar1 = *(char *)(iVar2 + 4);
    }
  }
  return cVar1;
}

