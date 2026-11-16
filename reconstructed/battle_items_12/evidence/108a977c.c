/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a977c(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  iVar2 = FUN_108544a0(param_2,param_3);
  bVar1 = *(byte *)(iVar2 + 0x152);
  uVar4 = 0;
  if (bVar1 != 0) {
    do {
      iVar3 = FUN_108520e4(iVar2,uVar4 & 0xff);
      if (iVar3 == 0) {
        if ((uVar4 & 0xff) == 4) {
          return;
        }
        FUN_1084c96c(param_1);
        FUN_1084c948(param_2,0,param_3);
        return;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < bVar1);
  }
  return;
}

