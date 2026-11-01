/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bf864(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  short sVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(4);
  if ((iVar2 == param_3) && (iVar2 = FUN_1084be58(param_2), iVar2 == 1)) {
    sVar1 = FUN_1086b91c(0x1e);
    iVar2 = FUN_1084bf04(sVar1);
    if ((iVar2 != 0) || (sVar1 == 0xe)) {
      uVar3 = FUN_108658d4(0x42,1);
      *param_4 = uVar3;
    }
  }
  return;
}

