/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b3618(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (*(int *)(param_4 + 0x18) != 0)) {
    iVar1 = FUN_10869f7c(param_2,10,param_3);
    *(undefined1 *)(iVar1 + 4) = 0x19;
    *(undefined1 *)(iVar1 + 0x11) = 1;
    *(char *)(iVar1 + 5) = (char)param_3;
    *(undefined1 *)(iVar1 + 0x12) = 1;
    FUN_10867b48(param_2);
    *(undefined4 *)(param_4 + 0x18) = 0;
  }
  return;
}

