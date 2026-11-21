/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108a4854(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  if (param_1 == 0x19) {
    return 0;
  }
  iVar1 = FUN_1086fe6c(param_1,&DAT_108d10f0,2);
  if (iVar1 == 0) {
    uVar2 = FUN_1086fe6c(param_1,&DAT_108d23b6,10);
    return uVar2;
  }
  return 1;
}

