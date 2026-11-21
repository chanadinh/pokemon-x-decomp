/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1088d98c(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  undefined4 unaff_r4;
  char cVar4;
  undefined1 uVar5;
  
  thunk_FUN_00333acc(*(undefined4 *)(param_1 + 0xe0),(int)*(char *)(param_1 + 0xd5));
  thunk_FUN_003356dc(*(undefined4 *)(param_1 + 0xe0));
  cVar4 = *(char *)(param_1 + 0xe8);
  if (cVar4 == '\0') {
    if (*(char *)(param_1 + 0xd5) != '\0') {
      FUN_1088db64(param_1);
      return;
    }
    iVar3 = thunk_FUN_00493cd8(*(undefined4 *)(param_1 + 0xe0));
    if (iVar3 == 0) {
      return;
    }
    uVar2 = thunk_FUN_00493d08(*(undefined4 *)(param_1 + 0xe0));
    *(undefined *)(param_1 + 0xd6) = (&DAT_108ce084)[(uVar2 & 0xff) * 0x18];
    uVar2 = thunk_FUN_00493d08(*(undefined4 *)(param_1 + 0xe0));
    cVar4 = '\x01';
    *(undefined *)(param_1 + 0xea) = (&DAT_108ce092)[(uVar2 & 0xff) * 0x18];
    uVar1 = 2;
  }
  else {
    if (cVar4 != '\x01') {
      if (cVar4 != '\x02') {
        FUN_10800394(0,0);
      }
      if (*(char *)(param_1 + 0xd6) == '\x06') {
        FUN_10800394(0,0);
      }
      *(undefined1 *)(param_1 + 0x24) = 1;
      return;
    }
    if ((*(byte *)(param_1 + 0xea) < 0x23) &&
       (iVar3 = thunk_FUN_0049558c(param_1 + 4,*(undefined4 *)(param_1 + 0xd8),
                                   *(byte *)(param_1 + 0xea),param_4,unaff_r4), iVar3 == 0)) {
      return;
    }
    cVar4 = *(char *)(param_1 + 0xe9);
    uVar5 = 3;
    if (cVar4 == '\0') {
      thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xe0));
      goto LAB_1088da68;
    }
    uVar1 = 3;
    if (cVar4 != '\x01' && cVar4 != '\x02') goto LAB_1088da68;
  }
  uVar5 = uVar1;
  thunk_FUN_00334048(*(undefined4 *)(param_1 + 0xe0));
LAB_1088da68:
  *(char *)(param_1 + 0xe8) = cVar4;
  *(undefined1 *)(param_1 + 0xe9) = uVar5;
  return;
}

