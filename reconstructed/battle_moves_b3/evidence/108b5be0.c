/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b5be0(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if ((iVar2 == param_3) && (iVar2 = FUN_1084ca78(param_2,param_3), iVar2 != 0)) {
    iVar2 = FUN_10869f7c(param_2,0x1f,param_3);
    *(undefined1 *)(iVar2 + 4) = 2;
    uVar1 = FUN_10873158(param_2,param_3);
    *(undefined1 *)(iVar2 + 5) = uVar1;
    FUN_10867b48(param_2,iVar2);
    iVar2 = FUN_10869f7c(param_2,0x12,param_3);
    *(char *)(iVar2 + 4) = (char)param_3;
    FUN_10867b48(param_2,iVar2);
    return;
  }
  return;
}

