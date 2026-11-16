/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084c8b8(undefined4 param_1,int param_2,int param_3,undefined4 param_4)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 unaff_r4;
  uint uVar5;
  undefined4 in_lr;
  
  iVar2 = FUN_1086b91c(2);
  if (iVar2 == param_3) {
    iVar2 = FUN_108544a0(param_2,param_3);
    bVar1 = *(byte *)(iVar2 + 0x152);
    uVar5 = 0;
    if (bVar1 != 0) {
      do {
        iVar3 = FUN_108520e4(iVar2,uVar5 & 0xff);
        if (iVar3 == 0) {
          if ((uVar5 & 0xff) == 4) {
            return;
          }
          FUN_1084c96c(param_1);
          uVar4 = FUN_10869f88(param_2 + 0x437c,0,param_3,param_4,unaff_r4,in_lr);
          FUN_10867b48(param_2,uVar4);
          return;
        }
        uVar5 = uVar5 + 1;
      } while (uVar5 < bVar1);
    }
  }
  return;
}

