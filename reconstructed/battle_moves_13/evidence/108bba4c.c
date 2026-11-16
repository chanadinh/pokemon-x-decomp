/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bba4c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  int *piVar3;
  undefined1 *puVar4;
  int iVar5;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(4);
    iVar2 = FUN_1084bfc0(uVar1);
    piVar3 = &DAT_10b11c00 + iVar2 * 0x44;
    iVar5 = *piVar3;
    if (iVar5 == 0) {
      piVar3 = *(int **)(&DAT_10b11c10 + iVar2 * 0x110);
    }
    if (iVar5 != 0 || piVar3 != (int *)0x0) {
      iVar5 = FUN_10869f7c(param_2,0x1a,param_3);
      *(char *)(iVar5 + 8) = (char)iVar2;
      *(undefined1 *)(iVar5 + 4) = 4;
      iVar2 = 1;
      puVar4 = (undefined1 *)(iVar5 + 5);
      *puVar4 = 0;
      do {
        iVar2 = iVar2 + -1;
        puVar4[1] = 0;
        puVar4 = puVar4 + 2;
        *puVar4 = 0;
      } while (iVar2 != 0);
      if (1 < *(byte *)(iVar5 + 4)) {
        *(byte *)(iVar5 + 5) = *(byte *)(iVar5 + 5) | 1;
      }
      if (1 < *(byte *)(iVar5 + 4)) {
        *(byte *)(iVar5 + 5) = *(byte *)(iVar5 + 5) | 2;
      }
      FUN_10867b48(param_2,iVar5);
      iVar2 = FUN_10869f7c(param_2,0x3b,param_3);
      *(undefined1 *)(iVar2 + 4) = 1;
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

