/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a5c9c(int param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(4);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x1e), iVar2 == 7)) {
    cVar1 = FUN_1086b91c(3);
    if (cVar1 != '\x1f') {
      iVar2 = FUN_10869f7c(param_2,0xb,param_3);
      *(char *)(iVar2 + 0xf) = cVar1;
      *(undefined2 *)(iVar2 + 4) = 7;
      uVar3 = FUN_10876e20(param_3);
      *(undefined4 *)(iVar2 + 8) = uVar3;
      *(undefined1 *)(iVar2 + 0xc) = 1;
      *(undefined1 *)(iVar2 + 0xd) = 1;
      FUN_1084d2d4(iVar2 + 0x14,2,0x14a);
      FUN_1084d29c(iVar2 + 0x14,cVar1);
      FUN_1084d29c(iVar2 + 0x14,*(undefined2 *)(param_1 + 0x38));
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

