/* Unreviewed Ghidra pseudocode; types require assembly review. */

/* WARNING: Removing unreachable block (ram,0x00168a10) */

ulonglong FUN_00150004(int param_1,int param_2,int param_3,uint param_4,int param_5)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  
  uVar1 = (int)((ulonglong)((param_3 + param_2 * 2 + (param_4 >> 2)) * param_1) *
                (ulonglong)DAT_00150030 >> 0x20) + 5;
  uVar2 = uVar1 & 0xffff;
  iVar3 = (int)*(char *)(param_5 * 5 + DAT_00168a98 + 2);
  if (iVar3 == -1) {
    return CONCAT44(DAT_00168a9c,(DAT_00168a94 & uVar2 * 0x5a) * DAT_00168a9c + DAT_00168a9c >> 0x16
                   );
  }
  if (iVar3 == 1) {
    return CONCAT44(DAT_00168a9c,(DAT_00168a94 & uVar2 * 0x6e) * DAT_00168a9c + DAT_00168a9c >> 0x16
                   );
  }
  return CONCAT44(iVar3,uVar1) & 0xffffffff0000ffff;
}

