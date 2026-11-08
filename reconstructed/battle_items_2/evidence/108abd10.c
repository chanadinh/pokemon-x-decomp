/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108abd10(undefined4 param_1,undefined4 param_2,undefined4 param_3,int *param_4)

{
  undefined4 uVar1;
  int iVar2;
  
  if (*param_4 == 0) {
    uVar1 = FUN_108544a0(param_2,param_3,param_3,param_4,param_4);
    iVar2 = FUN_10856a88(uVar1,0xf);
    if (iVar2 == 0) {
      FUN_1084c198(param_1,param_2,param_3,2,0);
    }
  }
  return;
}

