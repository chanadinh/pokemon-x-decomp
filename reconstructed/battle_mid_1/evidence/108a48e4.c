/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108a48e4(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086fe6c(param_1,&DAT_108d10f0,2);
  if (iVar1 == 0) {
    uVar2 = FUN_1086fe6c(param_1,&DAT_108d23b6,10);
    return uVar2;
  }
  return 1;
}

