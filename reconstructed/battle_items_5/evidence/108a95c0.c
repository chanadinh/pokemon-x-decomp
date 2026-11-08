/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a95c0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(3);
    iVar2 = FUN_10869f7c(param_2,0xb,uVar1);
    *(char *)(iVar2 + 0xf) = (char)param_3;
    *(undefined2 *)(iVar2 + 4) = 1;
    uVar3 = FUN_10860e00();
    *(undefined4 *)(iVar2 + 8) = uVar3;
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

