/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084aba4(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = FUN_108544a0();
  iVar2 = FUN_10856a88(uVar1,param_3);
  if (iVar2 != 0) {
    FUN_1084c948(param_1,1,param_2);
    iVar2 = FUN_10869f7c(param_1,10,param_2);
    *(char *)(iVar2 + 5) = (char)param_2;
    *(char *)(iVar2 + 4) = (char)param_3;
    *(undefined1 *)(iVar2 + 0x11) = 1;
    FUN_10867b48(param_1);
    FUN_1084c948(param_1,2,param_2);
    return;
  }
  return;
}

