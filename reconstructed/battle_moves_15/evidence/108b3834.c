/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b3834(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    iVar1 = FUN_10869f7c(param_2,0xb,param_3);
    *(char *)(iVar1 + 0xf) = (char)param_3;
    *(undefined1 *)(iVar1 + 0xd) = 1;
    *(undefined2 *)(iVar1 + 4) = 0x18;
    uVar2 = FUN_1084beac(1,2);
    *(undefined4 *)(iVar1 + 8) = uVar2;
    FUN_10876ca8(iVar1 + 8,1);
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

