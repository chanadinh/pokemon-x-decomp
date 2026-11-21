/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108acc94(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  
  uVar2 = FUN_1086b91c(4);
  iVar3 = FUN_1084bfc0(uVar2 & 0xff);
  if ((iVar3 == param_3) && (uVar4 = FUN_1086b91c(3), uVar4 != (uVar2 & 0xff))) {
    sVar1 = FUN_1086b91c(0x1e);
    iVar3 = FUN_1084bf04(sVar1);
    if ((iVar3 != 0) || ((sVar1 == 6 || (sVar1 == 0xe)))) {
      uVar5 = FUN_108658d4(0x42,1);
      *param_4 = uVar5;
    }
  }
  return;
}

