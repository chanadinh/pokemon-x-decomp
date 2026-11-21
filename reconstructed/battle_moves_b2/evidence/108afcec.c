/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108afcec(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    FUN_10855c68(uVar2,0);
    uVar3 = thunk_FUN_0039dfc4();
    iVar1 = FUN_1086f960(uVar2,uVar3);
    if (iVar1 == 0) {
      iVar1 = FUN_10869f7c(param_2,0x13,param_3);
      *(char *)(iVar1 + 6) = (char)param_3;
      *(ushort *)(iVar1 + 4) = (ushort)(uVar3 & 0x1f) | (ushort)((uVar3 & 0x1f) << 5) | 0x4800;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

