/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ba62c(undefined4 param_1,undefined4 param_2,int param_3,undefined1 *param_4)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  undefined1 *local_18;
  
  local_18 = param_4;
  iVar2 = FUN_1086b91c(2);
  if (iVar2 != param_3) {
    return;
  }
  uVar3 = FUN_1086b91c(0xe);
  uVar3 = uVar3 & 0xff;
  if (uVar3 == 10) {
    uVar4 = FUN_1084cb78(param_2,param_3);
    uVar5 = FUN_1084bf14(param_2);
    uVar3 = FUN_1086de1c(uVar5,uVar4,0);
  }
  iVar2 = FUN_1084bc20(param_2,uVar3,param_4);
  if (iVar2 != 0) {
    uVar4 = FUN_108544a0(param_2,*param_4);
    uVar1 = *param_4;
    iVar2 = FUN_1084c818(uVar4,3);
    if ((((iVar2 == 0) && (iVar2 = FUN_1084b784(param_2,uVar1,&local_18), iVar2 != 0)) &&
        (iVar2 = FUN_1084bfa0(param_2,uVar1), iVar2 != 0)) &&
       (((iVar2 = FUN_1084c030(param_2,uVar1), iVar2 == 0 &&
         (iVar2 = FUN_1084b648(&local_18), iVar2 != 0)) &&
        ((iVar6 = thunk_FUN_0039e058(), iVar6 != 0 && (iVar6 = FUN_108a4824(iVar2), iVar6 == 0))))))
    {
      FUN_108658d4(0x12,iVar2);
      FUN_108658d4(0xd,uVar3);
      return;
    }
  }
  FUN_108658d4(0x42,1);
  return;
}

