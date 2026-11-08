/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c4eb4(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  int iVar3;
  
  iVar3 = FUN_1084af30(param_2,param_3);
  if (iVar3 == 0) {
    uVar1 = FUN_1086b91c(2);
    iVar3 = FUN_1084c5a8(uVar1,param_3);
    if (iVar3 == 0) {
      uVar2 = FUN_1086b91c(0x12);
      iVar3 = thunk_FUN_0039dcf0(uVar2);
      if (((iVar3 == 0xb) && (iVar3 = thunk_FUN_0039df44(uVar2,4), iVar3 != 0)) &&
         (iVar3 = FUN_108658d4(0x23,0x19), iVar3 != 0)) {
        FUN_1084b3f8(param_2,param_3,uVar1);
        return;
      }
    }
  }
  return;
}

