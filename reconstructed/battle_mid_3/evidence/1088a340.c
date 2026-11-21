/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1088a340(int param_1)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  char cVar7;
  undefined1 uVar8;
  undefined8 uVar9;
  float fVar10;
  
  thunk_FUN_003356dc(*(undefined4 *)(param_1 + 0xe8));
  thunk_FUN_0014eb04();
  uVar4 = thunk_FUN_0010e07c();
  uVar4 = thunk_FUN_001172f4(uVar4,1);
  cVar7 = *(char *)(param_1 + 0xf4);
  if (cVar7 == '\0') {
    iVar6 = thunk_FUN_00493c9c(*(undefined4 *)(param_1 + 0xe8));
    if (iVar6 != 0) {
      uVar5 = thunk_FUN_00493cf8();
      thunk_FUN_00335dc0(*(undefined4 *)(param_1 + 0xec),uVar5 & 0xff);
      iVar6 = DAT_108f8334;
      uVar4 = *(undefined4 *)(param_1 + 0xe0);
      iVar2 = thunk_FUN_00497c60(uVar4,*(undefined4 *)(&UNK_108d065c + (uVar5 & 0xff) * 0x18));
      iVar3 = thunk_FUN_0049774c(uVar4);
      uVar9 = *(undefined8 *)(iVar2 + 0x28);
      fVar10 = *(float *)(iVar2 + 0x30);
      while (iVar2 = *(int *)(iVar2 + 0xc), iVar2 != 0 && iVar2 != *(int *)(iVar3 + 0x10)) {
        uVar9 = CONCAT44((float)((ulonglong)uVar9 >> 0x20) + *(float *)(iVar2 + 0x2c),
                         (float)uVar9 + *(float *)(iVar2 + 0x28));
        fVar10 = fVar10 + *(float *)(iVar2 + 0x30);
      }
      iVar6 = thunk_FUN_004977b0(*(undefined4 *)(iVar6 + 0xc),0);
      *(int *)(iVar6 + 0x28) = (int)uVar9;
      *(int *)(iVar6 + 0x2c) = (int)((ulonglong)uVar9 >> 0x20);
      *(float *)(iVar6 + 0x30) = fVar10;
      *(byte *)(iVar6 + 0xb7) = *(byte *)(iVar6 + 0xb7) & 0xcf;
      return;
    }
    iVar6 = thunk_FUN_00493cd8(*(undefined4 *)(param_1 + 0xe8));
    if (iVar6 == 0) {
      iVar6 = thunk_FUN_00493c8c(*(undefined4 *)(param_1 + 0xe8));
      if (iVar6 != 0) {
        return;
      }
      iVar6 = thunk_FUN_00497138(uVar4,0x20,0);
      if (iVar6 == 0) {
        iVar6 = thunk_FUN_00497138(uVar4,0x10,0);
        if (iVar6 == 0) {
          iVar6 = thunk_FUN_00493bc0(*(undefined4 *)(param_1 + 0xe8));
          if (iVar6 != 0) {
            return;
          }
          iVar6 = thunk_FUN_00335c84(*(undefined4 *)(param_1 + 0xec));
          if (iVar6 != -3) {
            return;
          }
          uVar5 = thunk_FUN_00494228(*(undefined4 *)(param_1 + 0xec));
          FUN_10853d20(*(undefined4 *)(param_1 + 0xe0),
                       *(undefined4 *)(&UNK_108d065c + (uVar5 & 0xff) * 0x18));
          iVar6 = 0;
          if (*(int **)(param_1 + 0x20) != (int *)0x0) {
            iVar6 = (**(code **)(**(int **)(param_1 + 0x20) + 0x34))();
          }
          if (iVar6 != 0) {
            return;
          }
          FUN_00425b1c(0x6000f,0,0xffffffff);
          return;
        }
        uVar1 = thunk_FUN_00494228(*(undefined4 *)(param_1 + 0xec));
        iVar6 = thunk_FUN_00493c38(*(undefined4 *)(param_1 + 0xe8),uVar1);
        if (iVar6 == 0) {
          return;
        }
        uVar4 = *(undefined4 *)(param_1 + 0xe8);
      }
      else {
        iVar6 = thunk_FUN_00493c38(*(undefined4 *)(param_1 + 0xe8),3);
        if (iVar6 == 0) {
          return;
        }
        uVar4 = *(undefined4 *)(param_1 + 0xe8);
        uVar1 = 3;
      }
      FUN_00334038(uVar4,uVar1);
      return;
    }
    uVar5 = thunk_FUN_00493d08();
    iVar6 = thunk_FUN_00493c38(*(undefined4 *)(param_1 + 0xe8),uVar5 & 0xff);
    if (iVar6 == 0) {
      return;
    }
    cVar7 = '\x01';
    iVar6 = (uVar5 & 0xff) * 0x18;
    *(undefined *)(param_1 + 0xdc) = (&UNK_108d0654)[iVar6];
    *(undefined *)(param_1 + 0xf6) = (&UNK_108d0662)[iVar6];
    uVar1 = 2;
  }
  else {
    if (cVar7 != '\x01') {
      if (cVar7 != '\x02') {
        FUN_10800394(0,0);
      }
      if (*(char *)(param_1 + 0xdc) == '\x04') {
        FUN_10800394(0,0);
      }
      *(undefined1 *)(param_1 + 0x24) = 1;
      return;
    }
    if ((*(byte *)(param_1 + 0xf6) < 0x1f) &&
       (iVar6 = thunk_FUN_0049558c(param_1 + 4,*(undefined4 *)(param_1 + 0xe0)), iVar6 == 0)) {
      return;
    }
    cVar7 = *(char *)(param_1 + 0xf5);
    uVar8 = 3;
    if (cVar7 == '\0') {
      thunk_FUN_00334174(*(undefined4 *)(param_1 + 0xe8));
      goto LAB_1088a5d8;
    }
    uVar1 = 3;
    if (cVar7 != '\x01' && cVar7 != '\x02') goto LAB_1088a5d8;
  }
  uVar8 = uVar1;
  thunk_FUN_00334048(*(undefined4 *)(param_1 + 0xe8));
LAB_1088a5d8:
  *(char *)(param_1 + 0xf4) = cVar7;
  *(undefined1 *)(param_1 + 0xf5) = uVar8;
  return;
}

