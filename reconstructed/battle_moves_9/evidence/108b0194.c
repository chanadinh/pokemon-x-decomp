/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b0194(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1086f960(uVar2,3);
    if (iVar1 != 0) {
      iVar1 = FUN_10869f7c(param_2,0xb,param_3);
      *(char *)(iVar1 + 0xf) = (char)param_3;
      *(undefined2 *)(iVar1 + 4) = 0x1c;
      uVar2 = FUN_1084be3c(1);
      *(undefined4 *)(iVar1 + 8) = uVar2;
      FUN_10867b48(param_2,iVar1);
      return;
    }
  }
  return;
}

