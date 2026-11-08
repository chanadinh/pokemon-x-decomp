/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bad6c(undefined4 param_1,undefined4 param_2,int param_3,undefined1 *param_4)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  int iVar5;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_1086b91c(0xe);
    uVar2 = uVar2 & 0xff;
    if (uVar2 == 10) {
      uVar3 = FUN_1084cb78(param_2,param_3);
      uVar4 = FUN_1084bf14(param_2);
      uVar2 = FUN_1086de1c(uVar4,uVar3,0);
    }
    iVar1 = FUN_1084bc20(param_2,uVar2,param_4);
    if (iVar1 != 0) {
      FUN_108544a0(param_2,*param_4);
      iVar1 = FUN_1084c7fc();
      if ((iVar1 != 0) && (iVar5 = thunk_FUN_0039df44(iVar1,6), iVar5 != 0)) {
        FUN_108658d4(0x12,iVar1);
        FUN_108658d4(0xd,uVar2);
        return;
      }
      FUN_108658d4(0x42,1);
      return;
    }
  }
  return;
}

