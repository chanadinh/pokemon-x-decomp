/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a8104(undefined4 param_1,undefined4 param_2,int param_3)

{
  ushort uVar1;
  int iVar2;
  bool bVar3;
  
  iVar2 = FUN_1086b91c(4);
  if (iVar2 == param_3) {
    iVar2 = FUN_108544a0(param_2,param_3);
    uVar1 = *(ushort *)(iVar2 + 0xc);
    bVar3 = uVar1 == 0x29e;
    if (bVar3) {
      uVar1 = (ushort)*(byte *)(iVar2 + 0x153);
    }
    if ((!bVar3 || uVar1 != 5) && (iVar2 = FUN_1081caf0(param_2,param_3), iVar2 != 0)) {
      FUN_1084ce84(0x36,0x1800);
      return;
    }
  }
  return;
}

