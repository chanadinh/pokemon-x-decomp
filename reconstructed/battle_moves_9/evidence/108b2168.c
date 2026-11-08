/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b2168(int param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (uVar2 = FUN_1086b91c(0x12), uVar2 == *(ushort *)(param_1 + 0x38))) {
    iVar1 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar1 + 4,2,0x298);
    FUN_1084d29c(iVar1 + 4,param_3);
    FUN_10867b48(param_2,iVar1);
    FUN_108658d4(0x3f,1);
    param_4[6] = 1;
    *param_4 = 1;
  }
  return;
}

