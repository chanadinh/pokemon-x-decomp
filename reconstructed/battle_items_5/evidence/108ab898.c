/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ab898(undefined4 param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    sVar1 = FUN_1086b91c(0x12);
    if (sVar1 != 0xa5) {
      iVar2 = FUN_10869f7c(param_2,0xb,param_3);
      *(char *)(iVar2 + 0xf) = (char)param_3;
      *(undefined2 *)(iVar2 + 4) = 0x1b;
      uVar3 = FUN_1084c3b0(sVar1);
      *(undefined4 *)(iVar2 + 8) = uVar3;
      *(undefined1 *)(iVar2 + 0x10) = 2;
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

