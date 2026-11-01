/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_1081f7a0(int param_1,int param_2)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  undefined4 uVar8;
  
  uVar8 = 0;
  uVar5 = 0;
  if (*(char *)(param_2 + 4) != '\0') {
    do {
      iVar3 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)(param_2 + uVar5 + 7));
      if (iVar3 != 10) {
        uVar6 = (uint)*(byte *)(param_2 + uVar5 + 7);
        iVar3 = *(int *)(param_1 + 8);
        if (0x17 < uVar6) {
          FUN_10800394(0,0);
        }
        iVar3 = iVar3 + uVar6 * 4;
        if (*(int *)(iVar3 + 0x84) == 0) {
          FUN_10800394(0,0);
        }
        iVar7 = *(int *)(iVar3 + 0x84);
        iVar3 = FUN_10874420(iVar7,0xd);
        if (iVar3 != 0) {
          cVar1 = *(char *)(param_2 + 5);
          uVar6 = *(uint *)(param_2 + uVar5 * 4 + 0x14);
          uVar2 = *(undefined1 *)(iVar7 + 0x19);
          if ((int)uVar6 < 1) {
            if ((int)uVar6 < 0) {
              FUN_10867a04(param_1,iVar7,-uVar6,0);
            }
          }
          else {
            uVar4 = (uint)*(ushort *)(iVar7 + 0x10) + (uVar6 & 0xffff);
            *(short *)(iVar7 + 0x10) = (short)uVar4;
            if ((uint)*(ushort *)(iVar7 + 0xe) < (uVar4 & 0xffff)) {
              *(ushort *)(iVar7 + 0x10) = *(ushort *)(iVar7 + 0xe);
            }
            FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),3,uVar2,uVar6 & 0xffff);
          }
          if ((cVar1 == '\0') && (iVar3 = FUN_1086c2cc(param_1 + 0x120c,uVar2), iVar3 != 10)) {
            FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x48,uVar2);
          }
          if ((*(char *)(param_2 + 6) == '\0') && (*(short *)(iVar7 + 0x12) != 0)) {
            uVar8 = FUN_1086b284((undefined4 *)(param_1 + 0x437c));
            FUN_1086bb84();
            FUN_1086bac0(2,*(undefined1 *)(iVar7 + 0x19));
            FUN_1086bac0(0x2f,1);
            FUN_1086b9a4(param_1,0x9c);
            FUN_1086b880();
            *(undefined4 *)(param_1 + 0x437c) = uVar8;
          }
          uVar8 = 1;
        }
      }
      uVar5 = uVar5 + 1;
    } while (uVar5 < *(byte *)(param_2 + 4));
  }
  return uVar8;
}

