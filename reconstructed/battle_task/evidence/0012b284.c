/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_2b284(int param_1,int param_2)

{
  int iVar1;
  bool bVar2;
  
  iVar1 = DAT_0012b2cc;
  bVar2 = param_1 == 0;
  if (-1 < param_1) {
    param_2 = *(int *)(DAT_0012b2cc + 0xcc);
    bVar2 = param_2 == param_1;
  }
  if ((!bVar2 && (-1 < param_1 && param_1 <= param_2)) &&
     ((*(uint *)(*(int *)(DAT_0012b2cc + 200) + param_1 * 8 + 4) & 1) != 0)) {
    return;
  }
  do {
    do {
    } while (*(code **)(iVar1 + 0x6c) == (code *)0x0);
    (**(code **)(iVar1 + 0x6c))(DAT_0012b2d0);
  } while( true );
}

