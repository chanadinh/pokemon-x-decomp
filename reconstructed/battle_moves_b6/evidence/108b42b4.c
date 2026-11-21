/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b42b4(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  
  iVar1 = FUN_10873158(param_2,param_3);
  iVar2 = FUN_1084bf14(param_2);
  iVar4 = param_3;
  if (iVar2 == 1) {
    uVar3 = FUN_1087425c(param_3);
    iVar2 = FUN_1081e28c(param_2,uVar3);
    if (iVar2 != 4) goto LAB_108b4360;
    iVar4 = FUN_1084b664(iVar1,0);
    if (iVar4 == iVar1) {
      iVar4 = FUN_1084b664(iVar1,1);
    }
  }
  else {
    if ((iVar2 != 2) || (iVar2 = FUN_10814f9c(iVar1), iVar2 != 0)) goto LAB_108b4360;
    iVar4 = FUN_1084b664(iVar1,0);
    if (iVar4 == iVar1) {
      iVar4 = FUN_1084b664(iVar1,2);
    }
  }
  iVar4 = FUN_1084d348(param_2,iVar4);
LAB_108b4360:
  if (iVar4 != param_3) {
    iVar1 = FUN_10869f7c(param_2,0x33,param_3);
    *(char *)(iVar1 + 4) = (char)param_3;
    *(char *)(iVar1 + 5) = (char)iVar4;
    FUN_1084d2d4(iVar1 + 8,2,0x477);
    FUN_1084d29c(iVar1 + 8,param_3);
    FUN_1084d29c(iVar1 + 8,iVar4);
    FUN_10867b48(param_2,iVar1);
    return;
  }
  return;
}

