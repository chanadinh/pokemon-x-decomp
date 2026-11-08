/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ba1dc(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(4);
    iVar2 = FUN_108544a0(param_2,uVar1);
    if ((*(short *)(iVar2 + 0x12) != 0) && (iVar2 = FUN_1084bb5c(param_2,param_3,uVar1), iVar2 == 0)
       ) {
      FUN_1084ce84(0x32,0x1800);
      return;
    }
  }
  return;
}

