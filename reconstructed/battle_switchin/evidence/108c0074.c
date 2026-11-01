/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0074(undefined4 param_1,undefined4 param_2,int param_3,undefined4 *param_4)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108544a0(param_2,param_3);
    iVar1 = FUN_1084c818(uVar2,0);
    if (iVar1 != 0) {
      *param_4 = 1;
    }
  }
  return;
}

