/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_001685f0(int param_1)

{
  char cVar1;
  undefined2 uVar2;
  bool bVar3;
  
  cVar1 = *(char *)(param_1 + 0xc);
  bVar3 = cVar1 == '\0';
  if (bVar3) {
    cVar1 = *(char *)(param_1 + 0xd);
  }
  if (bVar3 && cVar1 == '\0') {
    uVar2 = FUN_00174e0c(*(int *)(param_1 + 8) + 8,0xe0);
    *(undefined2 *)(*(int *)(param_1 + 8) + 6) = uVar2;
    FUN_00167fb4(*(undefined4 **)(param_1 + 8) + 2,0xe0,**(undefined4 **)(param_1 + 8));
    if (*(int *)(param_1 + 4) != 0) {
      FUN_00167fb4(*(int *)(param_1 + 4),0x1c,**(undefined4 **)(param_1 + 8));
    }
    *(undefined1 *)(param_1 + 0xc) = 1;
  }
  return;
}

