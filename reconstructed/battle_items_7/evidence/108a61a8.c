/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a61a8(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    iVar1 = FUN_10869f7c(param_2,0xb,param_3);
    *(undefined2 *)(iVar1 + 4) = 4;
    uVar2 = FUN_10860e00();
    *(undefined4 *)(iVar1 + 8) = uVar2;
    *(undefined1 *)(iVar1 + 0xc) = 0;
    *(char *)(iVar1 + 0xf) = (char)param_3;
    *(uint *)(iVar1 + 0x1c) = (uint)*(ushort *)(param_1 + 0x38);
    FUN_1084d2d4(iVar1 + 0x14,2,0x102);
    FUN_1084d29c(iVar1 + 0x14,param_3);
    FUN_1084d29c(iVar1 + 0x14,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

