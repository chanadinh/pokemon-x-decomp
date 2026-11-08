/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c3dc8(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((((iVar1 == param_3) && (uVar2 = FUN_1086b91c(0x10), uVar2 != *(ushort *)(param_1 + 0x38))) &&
      (iVar1 = FUN_108544a0(param_2,param_3), *(short *)(iVar1 + 0xc) == 0x15f)) &&
     (*(char *)(iVar1 + 0x153) != '\0')) {
    iVar1 = FUN_10869f7c(param_2,0x3a,param_3);
    *(char *)(iVar1 + 4) = (char)param_3;
    *(undefined1 *)(iVar1 + 5) = 0;
    FUN_1084d2d4(iVar1 + 8,2,0xde);
    FUN_1084d29c(iVar1 + 8,param_3);
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

