/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_00127c5c(int param_1,int param_2,undefined4 param_3,undefined4 param_4,undefined4 *param_5)

{
  int iVar1;
  undefined4 uVar2;
  
  FUN_0015f828(param_1 + 0x60);
  iVar1 = FUN_001336bc(param_1 + 0x18,0,param_2,param_2 >> 0x1f);
  if ((iVar1 < 0) && (iVar1 < 0)) {
    FUN_00124504(iVar1,0x127ca4);
  }
  uVar2 = FUN_00127b6c(param_1,param_3,param_4);
  if (param_5 != (undefined4 *)0x0) {
    *param_5 = uVar2;
  }
  FUN_0015f8f0(param_1 + 0x60);
  return;
}

