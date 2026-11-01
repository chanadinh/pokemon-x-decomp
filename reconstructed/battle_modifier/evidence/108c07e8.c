/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c07e8(undefined4 param_1,undefined4 param_2,uint param_3)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_1086b91c(4);
  if (((uVar1 & 0xff) != param_3) && (iVar2 = FUN_1084c5a8(param_3), iVar2 != 0)) {
    FUN_1084ce84(0x36,0xc00);
    return;
  }
  return;
}

