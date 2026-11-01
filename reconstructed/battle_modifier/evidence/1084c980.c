/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4
FUN_1084c980(undefined4 param_1,undefined4 param_2,int param_3,int *param_4,int param_5,int param_6)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if ((((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x16), iVar1 == param_5)) &&
      (iVar1 = FUN_1086b91c(0x47), iVar1 == 0)) &&
     (((param_6 != 0 || (iVar1 = FUN_1086b91c(0x39), 7 < iVar1)) && (*param_4 == 0)))) {
    FUN_1084ce84(0x36,0x800);
    iVar1 = FUN_1084c70c(param_2);
    if (iVar1 == 0) {
      *param_4 = 1;
      iVar1 = FUN_10869f7c(param_2,0x15,param_3);
      *(char *)(iVar1 + 5) = (char)param_3;
      *(undefined1 *)(iVar1 + 4) = 0x11;
      FUN_10867b48(param_2);
    }
    return 1;
  }
  return 0;
}

