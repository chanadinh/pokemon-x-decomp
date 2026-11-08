/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b4aec(undefined4 param_1,undefined4 param_2,int param_3,uint param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint local_18;
  
  local_18 = param_4;
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108a48a0(&local_18);
    uVar2 = FUN_1089e724(uVar2,local_18 & 0xffff);
    uVar3 = FUN_10871bac(param_2,param_3,uVar2);
    FUN_108658d4(0x12,uVar2);
    FUN_108658d4(0xd,uVar3);
  }
  return;
}

