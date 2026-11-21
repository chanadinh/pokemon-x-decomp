/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc0f4(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(0x16);
  if (iVar2 == 0xf) {
    uVar1 = FUN_1086b91c(4);
    iVar2 = FUN_1084bf24(param_2,uVar1);
    if ((iVar2 == 0) && (iVar2 = FUN_1084af30(param_2,uVar1), iVar2 == 0)) {
      FUN_1084ce84(0x32,0x800);
      return;
    }
  }
  return;
}

