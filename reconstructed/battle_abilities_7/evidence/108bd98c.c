/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bd98c(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined1 local_20 [12];
  
  uVar1 = FUN_1086b91c(2);
  iVar2 = FUN_1084c5a8(uVar1,param_3);
  if (iVar2 == 0) {
    uVar3 = FUN_1084cb78(param_2,param_3);
    uVar3 = FUN_1084bc20(param_2,uVar3 | 0x100,local_20);
    uVar5 = 0;
    if (uVar3 != 0) {
      do {
        uVar4 = FUN_108544a0(param_2,local_20[uVar5]);
        iVar2 = FUN_1086f960(uVar4,8);
        if (iVar2 != 0) {
          FUN_108658d4(0x42,1);
        }
        uVar5 = uVar5 + 1 & 0xff;
      } while (uVar5 < uVar3);
    }
  }
  return;
}

