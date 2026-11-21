/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b787c(undefined4 param_1,undefined4 param_2,int param_3,uint *param_4)

{
  undefined4 uVar1;
  uint uVar2;
  int iVar3;
  
  uVar1 = FUN_1087425c(param_3);
  uVar2 = FUN_10874634(param_2,uVar1);
  if ((1 < uVar2) && (iVar3 = FUN_1086b91c(3), iVar3 == param_3)) {
    uVar2 = FUN_1086b91c(6);
    uVar2 = uVar2 & 0xff;
    uVar1 = FUN_108544a0(param_2,uVar2);
    iVar3 = FUN_1085f844();
    if ((iVar3 == 0) && (iVar3 = FUN_1084c818(uVar1,1), iVar3 == 0)) {
      iVar3 = FUN_10869f7c(param_2,3,param_3);
      FUN_1084d2d4(iVar3 + 4,2,0x41a);
      FUN_1084d29c(iVar3 + 4,param_3);
      FUN_1084d29c(iVar3 + 4,uVar2);
      FUN_10867b48(param_2,iVar3);
      *param_4 = uVar2;
      FUN_1084c700(param_1);
      return;
    }
  }
  return;
}

