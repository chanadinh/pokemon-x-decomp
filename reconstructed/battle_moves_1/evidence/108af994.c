/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108af994(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    puVar2 = (undefined4 *)FUN_108544a0(param_2,param_3);
    iVar1 = thunk_FUN_004a0030(*puVar2);
    uVar3 = (uint)(iVar1 * 10) / 0x19;
    if (uVar3 == 0) {
      uVar3 = 1;
    }
    FUN_108658d4(0x31,uVar3);
    return;
  }
  return;
}

