/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_2b27c(void)

{
  int iVar1;
  
  iVar1 = DAT_0012b2cc;
  if ((8 < *(int *)(DAT_0012b2cc + 0xcc)) &&
     ((*(uint *)(*(int *)(DAT_0012b2cc + 200) + 0x44) & 1) != 0)) {
    return;
  }
  do {
    do {
    } while (*(code **)(iVar1 + 0x6c) == (code *)0x0);
    (**(code **)(iVar1 + 0x6c))(DAT_0012b2d0);
  } while( true );
}

