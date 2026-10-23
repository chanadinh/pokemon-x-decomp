/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0016830c(undefined4 param_1,undefined1 param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_00176168(param_1,0);
  if (*(char *)(iVar1 + 0x1b) == '\0') {
    iVar1 = FUN_001761e8(param_1,1);
    *(undefined1 *)(iVar1 + 0x1a) = param_2;
  }
  else {
    iVar1 = FUN_00176168();
    *(undefined1 *)(iVar1 + 0x2a) = param_2;
  }
  FUN_001685f0(param_1);
  return;
}

