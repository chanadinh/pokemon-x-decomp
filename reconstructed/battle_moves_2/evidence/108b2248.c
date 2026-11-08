/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b2248(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    uVar2 = FUN_1081cc04(param_2);
    uVar3 = FUN_108c99fc(uVar2,1);
    uVar5 = 0;
    if (uVar3 != 0) {
      do {
        uVar4 = FUN_108c9a30(uVar2,1,uVar5);
        iVar1 = FUN_1084c5a8(param_3,uVar4);
        if (iVar1 != 0) {
          FUN_1084ce84(0x32,0x2000);
          return;
        }
        uVar5 = uVar5 + 1 & 0xff;
      } while (uVar5 < uVar3);
    }
  }
  return;
}

