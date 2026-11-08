/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b15c8(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1084b8d4(param_2), iVar1 != 3)) {
    iVar1 = FUN_10869f7c(param_2,0x1c,param_3);
    *(undefined1 *)(iVar1 + 5) = 3;
    *(undefined1 *)(iVar1 + 4) = 10;
    uVar2 = FUN_1084be3c(5);
    *(undefined4 *)(iVar1 + 8) = uVar2;
    FUN_1084d2d4(iVar1 + 0xc,1,0xd1);
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

