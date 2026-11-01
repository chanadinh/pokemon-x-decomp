/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bde48(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  *param_4 = 0;
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x16), iVar1 == 0)) {
    uVar2 = FUN_108658d4(0x16,2);
    *param_4 = uVar2;
  }
  return;
}

