/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b0fdc(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  byte bVar2;
  undefined1 uVar3;
  ushort uVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  int iVar8;
  uint uVar9;
  
  iVar5 = FUN_1086b91c(3);
  if (iVar5 == param_3) {
    uVar3 = FUN_1086b91c(6);
    iVar5 = FUN_108544a0(param_2,uVar3);
    cVar1 = *(char *)(iVar5 + 0x157);
    if (cVar1 != '\x12') {
      iVar5 = FUN_1084c7d0(param_2,0x12);
      uVar6 = FUN_1089e5ac(cVar1,iVar5);
      if (uVar6 != 0) {
        uVar7 = FUN_108544a0(param_2,param_3);
        uVar9 = 0;
        if (uVar6 != 0) {
          do {
            iVar8 = FUN_1086f960(uVar7,*(undefined1 *)(iVar5 + uVar9));
            if (iVar8 == 0) {
              uVar9 = uVar9 + 1;
            }
            else {
              if (0 < (int)((uVar6 - uVar9) + -1)) {
                thunk_FUN_00300ef8(iVar5 + uVar9,iVar5 + uVar9 + 1);
              }
              uVar6 = uVar6 - 1;
            }
          } while (uVar9 < uVar6);
          if (uVar6 == 0) {
            return;
          }
        }
        iVar8 = FUN_1084c7bc(uVar6);
        bVar2 = *(byte *)(iVar5 + iVar8);
        iVar5 = FUN_10869f7c(param_2,0x13,param_3);
        uVar4 = bVar2 & 0x1f;
        *(char *)(iVar5 + 6) = (char)param_3;
        *(ushort *)(iVar5 + 4) = uVar4 | uVar4 << 5 | 0x4800;
        FUN_10867b48(param_2);
        return;
      }
    }
  }
  return;
}

