/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108aa970(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  
  iVar2 = FUN_1086b91c(2);
  if ((iVar2 == param_3) && (uVar3 = FUN_1086b91c(0x2f), (uVar3 & 2) != 0)) {
    iVar2 = FUN_108544a0(param_2,param_3);
    bVar1 = *(byte *)(iVar2 + 0x152);
    uVar3 = 0;
    if (bVar1 != 0) {
      do {
        iVar4 = FUN_108520e4(iVar2,uVar3 & 0xff);
        if (iVar4 == 0) {
          if ((uVar3 & 0xff) == 4) {
            return;
          }
          FUN_1084c96c(param_1);
          FUN_1084c948(param_2,0,param_3);
          return;
        }
        uVar3 = uVar3 + 1;
      } while (uVar3 < bVar1);
    }
  }
  return;
}

