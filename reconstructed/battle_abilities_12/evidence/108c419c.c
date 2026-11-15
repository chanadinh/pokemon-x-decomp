/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c419c(int param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_1086b91c(0x10);
  if ((uVar1 != *(ushort *)(param_1 + 0x38)) && (iVar2 = FUN_1086b91c(2), iVar2 == param_3)) {
    FUN_108544a0(param_2,param_3);
    iVar2 = FUN_1084aa9c();
    if (iVar2 != 0) {
      iVar2 = FUN_10869f7c(param_2,0x35,param_3);
      *(char *)(iVar2 + 4) = (char)param_3;
      FUN_1084d2d4(iVar2 + 8,2,0x1de);
      FUN_1084d29c(iVar2 + 8,param_3);
      FUN_10867b48(param_2,iVar2);
      return;
    }
  }
  return;
}

