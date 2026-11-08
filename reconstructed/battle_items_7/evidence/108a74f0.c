/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a74f0(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    iVar1 = FUN_10869f7c(param_2,0xb,param_3);
    *(undefined2 *)(iVar1 + 4) = 5;
    *(undefined4 *)(iVar1 + 8) = 0x79;
    *(char *)(iVar1 + 0xf) = (char)param_3;
    *(undefined1 *)(iVar1 + 0xc) = 0;
    FUN_1084d2d4(iVar1 + 0x14,2,0xf0);
    FUN_1084d29c(iVar1 + 0x14,param_3);
    FUN_1084d29c(iVar1 + 0x14,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

