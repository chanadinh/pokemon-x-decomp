/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b2630(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    iVar2 = FUN_10869f7c(param_2,0xf,param_3);
    uVar3 = FUN_1084cb78(param_2,param_3);
    uVar1 = FUN_1084bc20(param_2,uVar3 | 0x800,iVar2 + 5);
    *(undefined1 *)(iVar2 + 4) = uVar1;
    FUN_10867b48(param_2,iVar2);
    iVar2 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar2 + 4,1,0x69);
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

