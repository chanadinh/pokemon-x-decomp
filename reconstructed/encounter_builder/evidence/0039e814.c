/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 _static_::anon_segment_0_29e814(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = anon_segment_0_3a16ac(param_2);
  if (((iVar1 == 0) && (*(byte *)(param_1 + 0x18) < 6)) &&
     (iVar1 = *(int *)(param_1 + (uint)*(byte *)(param_1 + 0x18) * 4), iVar1 != 0)) {
    anon_segment_0_29a52c(iVar1,param_2);
    *(char *)(param_1 + 0x18) = *(char *)(param_1 + 0x18) + '\x01';
    iVar1 = anon_segment_0_3a1658(*(undefined4 *)(param_1 + (uint)*(byte *)(param_1 + 0x19) * 4),2);
    if (iVar1 != 0) {
      iVar1 = 0;
      if (*(char *)(param_1 + 0x18) != '\0') {
        do {
          iVar2 = anon_segment_0_3a1658(*(undefined4 *)(param_1 + iVar1 * 4),2);
          if (iVar2 == 0) {
            *(char *)(param_1 + 0x19) = (char)iVar1;
            return 1;
          }
          iVar1 = iVar1 + 1;
        } while (iVar1 < (int)(uint)*(byte *)(param_1 + 0x18));
      }
      *(undefined1 *)(param_1 + 0x19) = 0;
    }
    return 1;
  }
  return 0;
}

