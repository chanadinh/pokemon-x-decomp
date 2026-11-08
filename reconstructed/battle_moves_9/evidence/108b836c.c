/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b836c(undefined4 param_1,undefined4 param_2,int param_3,uint *param_4)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 != param_3) {
    return;
  }
  bVar1 = FUN_1084c7bc(100);
  uVar3 = 0;
  do {
    if (bVar1 < (byte)(&DAT_108d4aa8)[uVar3 * 2]) break;
    uVar3 = uVar3 + 1 & 0xff;
  } while (uVar3 < 7);
  *param_4 = (uint)(byte)(&UNK_108d4aa9)[uVar3 * 2];
  iVar2 = FUN_10869f7c(param_2,3,param_3);
  FUN_1084d2d4(iVar2 + 4,1,uVar3 + 0x6c);
  FUN_10867b48(param_2,iVar2);
  return;
}

