/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108afb0c(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  char cVar4;
  undefined1 auStack_18 [2];
  short local_16;
  char local_13;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_108544a0(param_2,param_3);
    cVar1 = FUN_1086b91c(4);
    iVar2 = FUN_1084bac4(uVar3,0,0,auStack_18);
    cVar4 = '\x01';
    if (iVar2 != 0) {
      while ((local_13 != cVar1 || (local_16 == 0))) {
        iVar2 = FUN_1084bac4(uVar3,0,cVar4,auStack_18);
        cVar4 = cVar4 + '\x01';
        if (iVar2 == 0) {
          return;
        }
      }
      iVar2 = FUN_1086b91c(0x31);
      FUN_108658d4(0x31,iVar2 << 1);
    }
  }
  return;
}

