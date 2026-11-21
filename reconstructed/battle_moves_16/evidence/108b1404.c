/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b1404(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x47), iVar2 == 0)) {
    uVar1 = FUN_1086b91c(4);
    iVar2 = FUN_10869f7c(param_2,0xf,param_3);
    *(undefined1 *)(iVar2 + 5) = uVar1;
    *(undefined1 *)(iVar2 + 4) = 1;
    FUN_10867b48(param_2);
    iVar2 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar2 + 4,2,0xc3);
    FUN_1084d29c(iVar2 + 4,uVar1);
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

