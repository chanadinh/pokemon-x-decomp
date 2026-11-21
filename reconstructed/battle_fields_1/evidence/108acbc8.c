/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108acbc8(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  
  uVar1 = FUN_1086b91c(2);
  iVar2 = FUN_1084bfc0(uVar1 & 0xff);
  if (((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x21), iVar2 < 0)) &&
     (uVar3 = FUN_1086b91c(3), uVar3 != (uVar1 & 0xff))) {
    uVar4 = FUN_108658d4(0x42,1);
    *param_4 = uVar4;
  }
  return;
}

