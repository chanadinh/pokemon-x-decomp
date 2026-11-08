/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b98e8(undefined4 param_1,undefined4 param_2,uint param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  uint uVar3;
  int iVar4;
  
  uVar3 = FUN_1086b91c(2);
  if ((uVar3 & 0xff) == param_3) {
    iVar4 = FUN_1084b8e0(param_2,param_3);
    if (iVar4 != 0) {
      FUN_108658d4(0x23,0x1b);
      return;
    }
  }
  else {
    iVar4 = FUN_1084c5a8(param_3);
    if ((iVar4 == 0) && (iVar4 = FUN_1084af30(param_2,param_3), iVar4 == 0)) {
      uVar1 = FUN_1086b91c(2);
      iVar4 = FUN_1084c5a8(uVar1,param_3);
      if (iVar4 == 0) {
        uVar2 = FUN_1086b91c(0x12);
        iVar4 = thunk_FUN_0039dcf0(uVar2);
        if (((iVar4 == 0xb) && (iVar4 = thunk_FUN_0039df44(uVar2,4), iVar4 != 0)) &&
           (iVar4 = FUN_108658d4(0x23,0x19), iVar4 != 0)) {
          FUN_1084b3f8(param_2,param_3,uVar1);
          return;
        }
      }
    }
  }
  return;
}

