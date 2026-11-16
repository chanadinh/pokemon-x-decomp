/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a8a30(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_108722e8(uVar2,4,1);
    if (iVar1 != 0) {
      iVar1 = FUN_10869f7c(param_2,0xd,param_3);
      *(undefined1 *)(iVar1 + 0x16) = 1;
      *(undefined1 *)(iVar1 + 4) = 4;
      *(undefined1 *)(iVar1 + 0x17) = 1;
      *(char *)(iVar1 + 0xc) = (char)param_3;
      FUN_10867b48(param_2);
      return;
    }
  }
  return;
}

