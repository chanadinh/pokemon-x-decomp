/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b0c58(undefined4 param_1,undefined4 param_2,uint param_3)

{
  byte bVar1;
  byte bVar2;
  undefined2 uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined4 uVar9;
  byte bVar10;
  undefined2 *puVar11;
  byte bVar12;
  
  uVar4 = FUN_1086b91c(2);
  if (uVar4 == param_3) {
    iVar5 = FUN_1084b950(param_2,param_3);
    iVar6 = FUN_1084c7d0(param_2,0x30);
    uVar4 = 0;
    bVar1 = *(byte *)(iVar5 + 0x18);
    bVar12 = 0;
    if (bVar1 != 0) {
      do {
        iVar7 = FUN_1085daf8(iVar5,bVar12);
        if ((*(byte *)(iVar7 + 0x19) != param_3) &&
           ((iVar8 = FUN_1085f844(iVar7), iVar8 != 0 || (iVar8 = FUN_108551d4(iVar7), iVar8 != 0))))
        {
          bVar2 = *(byte *)(iVar7 + 0x152);
          bVar10 = 0;
          if (bVar2 != 0) {
            do {
              uVar3 = FUN_10855c68(iVar7,bVar10);
              iVar8 = FUN_108a47b4();
              if (iVar8 == 0) {
                iVar8 = uVar4 * 2;
                uVar4 = uVar4 + 1 & 0xff;
                *(undefined2 *)(iVar6 + iVar8) = uVar3;
              }
              bVar10 = bVar10 + 1;
            } while (bVar10 < bVar2);
          }
        }
        bVar12 = bVar12 + 1;
      } while (bVar12 < bVar1);
      if (uVar4 != 0) {
        uVar4 = FUN_1084c7bc(uVar4);
        puVar11 = (undefined2 *)(iVar6 + (uVar4 & 0xff) * 2);
        uVar9 = FUN_10871bac(param_2,param_3,*puVar11);
        FUN_108658d4(0x12,*puVar11);
        FUN_108658d4(0xd,uVar9);
        return;
      }
    }
  }
  return;
}

