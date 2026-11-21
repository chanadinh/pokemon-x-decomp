/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a85fc(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  uVar1 = FUN_1086b91c(2);
  iVar2 = FUN_1084c5a8(uVar1,param_3);
  if ((iVar2 != 0) && (iVar2 = FUN_108658d4(0x53,1), iVar2 != 0)) {
    iVar2 = FUN_10869f7c(param_2,3,param_3);
    FUN_1084d2d4(iVar2 + 4,2,0x3f2);
    FUN_1084d29c(iVar2 + 4,param_3);
    FUN_1084d29c(iVar2 + 4,*(undefined2 *)(param_1 + 0x38));
    FUN_10876c28(iVar2 + 4,0xd0024);
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

