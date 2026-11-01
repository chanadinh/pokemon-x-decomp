/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b9a54(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    puVar2 = (undefined4 *)FUN_108544a0(param_2,param_3);
    if (puVar2 == (undefined4 *)0x0) {
      FUN_10800394(0,0);
      uVar3 = 0;
    }
    else {
      uVar3 = thunk_FUN_004a0fdc(*puVar2);
    }
    FUN_108658d4(0x16,uVar3);
    return;
  }
  return;
}

