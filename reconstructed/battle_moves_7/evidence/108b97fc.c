/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b97fc(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    cVar1 = FUN_1086b91c(6);
    if (cVar1 != '\x1f') {
      uVar3 = FUN_108544a0(param_2,cVar1);
      iVar2 = FUN_10856a88(uVar3,1);
      if (iVar2 != 0) {
        iVar2 = FUN_10869f7c(param_2,10,param_3);
        *(char *)(iVar2 + 5) = cVar1;
        *(undefined1 *)(iVar2 + 0x11) = 1;
        *(undefined1 *)(iVar2 + 4) = 1;
        FUN_10867b48(param_2,iVar2);
        return;
      }
    }
  }
  return;
}

