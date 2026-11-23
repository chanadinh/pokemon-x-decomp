/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1083e4c4(int *param_1,int param_2)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  
  if (DAT_108fcb84 != 0x11ca) {
    DAT_108fcb84 = 0x11ca;
  }
  FUN_1086fd18((undefined1 *)((int)param_1 + 0x6ca),0xff);
  *(undefined1 *)((int)param_1 + 0x25d) = 0;
  uVar1 = (undefined1)param_2;
  *(undefined1 *)(param_1 + 0x1b2) = uVar1;
  *(undefined1 *)((int)param_1 + 0x6c9) = uVar1;
  *(undefined1 *)((int)param_1 + 0x6ca) = uVar1;
  iVar2 = (**(code **)(*param_1 + 0x20))(param_1,param_2);
  if (*(short *)(iVar2 + 0xc) == 0x2a9) {
    if (*(char *)(iVar2 + 0x153) == '\0') {
      iVar2 = 0x285;
    }
    else {
      iVar2 = 0x284;
    }
  }
  else {
    iVar2 = 0x272;
  }
  param_1[0x1b8] = iVar2;
  *(undefined1 *)(param_1 + 0x1c9) = *(undefined1 *)(param_1[param_2 + 0x40] + 0x15c);
  iVar3 = (**(code **)(*param_1 + 0xbc))(param_1);
  if ((iVar3 == 0) && (iVar3 = (**(code **)(*(int *)param_1[param_2 + 0x40] + 0x7c))(), iVar3 != 0))
  {
    param_1[0x1ae] = 0;
  }
  else {
    if (iVar2 == 0x285 || iVar2 == 0x284) {
      iVar2 = 0x78;
    }
    else {
      iVar2 = 100;
    }
    param_1[0x1ae] = iVar2;
  }
  return;
}

