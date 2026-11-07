/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b05f4(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 != param_3) {
    return;
  }
  uVar2 = FUN_108544a0(param_2,param_3);
  uVar3 = FUN_10874420(uVar2,0xe);
  iVar1 = FUN_10874420(uVar2,0xd);
  uVar4 = thunk_FUN_003006f8(iVar1 * 0x30,uVar3);
  if (uVar4 == 0) {
    uVar4 = 1;
  }
  uVar5 = 0;
  do {
    iVar1 = uVar5 * 2;
    if ((ushort)(&DAT_108d494c)[iVar1] < uVar4) {
      uVar5 = uVar5 + 1;
    }
  } while ((ushort)(&DAT_108d494c)[iVar1] < uVar4 && uVar5 < 5);
  FUN_108658d4(0x31,(&DAT_108d494e)[uVar5 * 2]);
  return;
}

