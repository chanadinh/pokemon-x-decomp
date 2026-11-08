/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b9e78(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = FUN_1084b8e0(param_2,param_3);
  if ((iVar1 != 0) && (iVar1 = FUN_108658d4(0x23,0x1b), iVar1 != 0)) {
    iVar1 = FUN_10869f7c(param_2,0x27,param_3);
    *(undefined1 *)(iVar1 + 5) = 3;
    *(char *)(iVar1 + 4) = (char)param_3;
    *(undefined1 *)(iVar1 + 6) = 0;
    FUN_10867b48(param_2);
    return;
  }
  return;
}

