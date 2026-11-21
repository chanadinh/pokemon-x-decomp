/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ae9c8(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(6);
    iVar2 = FUN_10869f7c(param_2,0xb,param_3);
    *(char *)(iVar2 + 0xf) = (char)param_3;
    *(undefined2 *)(iVar2 + 4) = 0x1d;
    uVar3 = FUN_108797b0(uVar1,2);
    *(undefined4 *)(iVar2 + 8) = uVar3;
    FUN_1084d2d4(iVar2 + 0x14,2,0x28b);
    FUN_1084d29c(iVar2 + 0x14,param_3);
    FUN_1084d29c(iVar2 + 0x14,uVar1);
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

