/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aa074(int param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  if ((*(int *)(param_4 + 0x18) != 0) && (iVar1 = FUN_1086b91c(2), iVar1 == param_3)) {
    FUN_108544a0(param_2,param_3);
    iVar1 = FUN_10869f7c(param_2,0xb,param_3);
    *(char *)(iVar1 + 0xf) = (char)param_3;
    *(undefined2 *)(iVar1 + 4) = 0x22;
    uVar2 = FUN_1084c3b0(0x78);
    *(undefined4 *)(iVar1 + 8) = uVar2;
    FUN_1084d2d4(iVar1 + 0x14,2,0x40a);
    FUN_1084d29c(iVar1 + 0x14,param_3);
    FUN_1084d29c(iVar1 + 0x14,*(undefined2 *)(param_1 + 0x38));
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

