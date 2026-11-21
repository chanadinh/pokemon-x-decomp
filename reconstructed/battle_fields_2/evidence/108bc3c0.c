/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bc3c0(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  uVar1 = FUN_1086b91c(4);
  iVar3 = FUN_1084bf24(param_2,uVar1);
  if ((iVar3 == 0) && (iVar3 = FUN_1084af30(param_2,uVar1), iVar3 == 0)) {
    uVar2 = FUN_1086b91c(0x1e);
    iVar3 = FUN_1084bf04(uVar2);
    if ((iVar3 != 0) && (iVar3 = FUN_108658d4(0x42,1), iVar3 != 0)) {
      iVar3 = FUN_10869f7c(param_2,0x17,uVar1);
      *(undefined1 *)(iVar3 + 5) = uVar1;
      *(undefined1 *)(iVar3 + 4) = 0x11;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

