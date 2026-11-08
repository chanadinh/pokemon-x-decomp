/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c340c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar2 = FUN_1086b91c(2);
  if ((iVar2 == param_3) && (iVar2 = FUN_1086b91c(0x3a), iVar2 == 1)) {
    uVar3 = FUN_108544a0(param_2,param_3);
    uVar1 = FUN_1084d2f4(uVar3,8);
    FUN_1084c948(param_2,1,param_3);
    iVar2 = FUN_10869f7c(param_2,6,param_3);
    *(char *)(iVar2 + 6) = (char)param_3;
    *(undefined2 *)(iVar2 + 4) = uVar1;
    FUN_10867b48(param_2,iVar2);
    FUN_1084c948(param_2,2,param_3);
    return;
  }
  return;
}

