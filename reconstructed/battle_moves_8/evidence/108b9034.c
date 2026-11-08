/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b9034(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(4);
    iVar2 = FUN_1084bf24(param_2,uVar1);
    if (iVar2 != 0) {
      FUN_108658d4(0x41,1);
      return;
    }
  }
  return;
}

