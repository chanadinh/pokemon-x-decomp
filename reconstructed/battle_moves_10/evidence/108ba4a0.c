/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ba4a0(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 != param_3) {
    return;
  }
  iVar1 = FUN_108544a0(param_2,param_3);
  if (((*(short *)(iVar1 + 0x12) != 0) &&
      (iVar1 = FUN_1084cf74(*(short *)(iVar1 + 0x12),6), iVar1 != 0)) &&
     (iVar1 = FUN_1084b5b8(param_2,param_3), iVar1 != 0)) {
    iVar1 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1085441c(param_2,*(undefined2 *)(iVar1 + 0xc),*(undefined2 *)(iVar1 + 0x12));
    if (iVar1 == 0) {
      return;
    }
  }
  FUN_108658d4(0x23,0x1b);
  return;
}

