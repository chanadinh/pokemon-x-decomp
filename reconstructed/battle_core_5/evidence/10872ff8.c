/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int * FUN_10872ff8(undefined4 param_1,int param_2,int *param_3)

{
  uint uVar1;
  uint uVar2;
  int *piVar3;
  int iVar4;
  
  if ((*(byte *)((int)param_3 + 0x1b) & 0x40) != 0) {
    uVar1 = FUN_1087425c(*(undefined1 *)((int)param_3 + 0x19));
    iVar4 = param_3[1];
    if ((iVar4 == 0) || ((*(byte *)((int)param_3 + 0x1b) & 0x40) == 0)) {
      iVar4 = *param_3;
    }
    if (3 < uVar1) {
      FUN_10800394(0,0);
    }
    param_2 = param_2 + uVar1 * 0x1c;
    uVar1 = 0;
    uVar2 = (uint)*(byte *)(param_2 + 0x1c);
    if (uVar2 != 0) {
      do {
        if ((uVar1 & 0xff) < uVar2) {
          piVar3 = *(int **)(param_2 + 4 + (uVar1 & 0xff) * 4);
        }
        else {
          piVar3 = (int *)0x0;
        }
        if (*piVar3 == iVar4) {
          return piVar3;
        }
        uVar1 = uVar1 + 1;
      } while (uVar1 < uVar2);
    }
  }
  return param_3;
}

