/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_1081d770(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  
  iVar2 = *(int *)(param_1 + 8);
  if (0x17 < param_2) {
    FUN_10800394(0,0);
  }
  iVar2 = iVar2 + param_2 * 4;
  if (*(int *)(iVar2 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar1 = *(int *)(iVar2 + 0x84);
  iVar2 = FUN_1086f960(iVar1,7);
  if (iVar2 == 0) {
    iVar2 = FUN_10874420(iVar1,0xd);
    if (iVar2 == 0) {
      bVar3 = false;
    }
    else {
      FUN_1086bb84();
      FUN_1086bac0(2,*(undefined1 *)(iVar1 + 0x19));
      FUN_1086b9fc(0x53,0);
      FUN_1086b9a4(param_1,0xb);
      iVar2 = FUN_1086b91c(0x53);
      bVar3 = iVar2 != 0;
      FUN_1086b880();
    }
  }
  else {
    bVar3 = true;
  }
  return bVar3;
}

