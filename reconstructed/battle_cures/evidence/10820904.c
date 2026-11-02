/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10820904(int param_1,uint *param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  undefined4 *puVar10;
  uint uVar11;
  undefined4 uVar12;
  undefined4 local_3c;
  undefined4 *local_38;
  int local_30;
  uint *puStack_2c;
  undefined4 local_28;
  
  uVar11 = (*param_2 & 0x1fff) >> 8;
  iVar7 = *(int *)(param_1 + 8);
  local_30 = param_1;
  puStack_2c = param_2;
  local_28 = param_3;
  if (0x17 < uVar11) {
    FUN_10800394(0,0);
  }
  iVar7 = iVar7 + uVar11 * 4;
  if (*(int *)(iVar7 + 0x84) == 0) {
    FUN_10800394(0,0);
  }
  iVar7 = *(int *)(iVar7 + 0x84);
  uVar12 = 0;
  if ((*param_2 & 0x800000) != 0) {
    FUN_1086c22c(*(undefined4 *)(local_30 + 0xc),0x66,*(undefined1 *)(iVar7 + 0x19));
  }
  iVar2 = local_30;
  uVar11 = 0;
  if (*(char *)((int)param_2 + 0x11) != '\0') {
    local_38 = (undefined4 *)(local_30 + 0x437c);
    puVar3 = (undefined4 *)(local_30 + 0x433c);
    do {
      uVar5 = (uint)*(byte *)((int)param_2 + uVar11 + 5);
      iVar8 = *(int *)(local_30 + 8);
      if (0x17 < uVar5) {
        FUN_10800394(0,0);
      }
      iVar8 = iVar8 + uVar5 * 4;
      if (*(int *)(iVar8 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      puVar10 = *(undefined4 **)(iVar8 + 0x84);
      iVar8 = thunk_FUN_004a1658(*puVar10,2);
      if ((iVar8 == 0) && (iVar8 = FUN_10874420(puVar10,0xd), iVar8 != 0)) {
        uVar5 = (uint)(byte)param_2[1];
        while( true ) {
          uVar9 = 0;
          uVar4 = FUN_1086d8ac(puVar10);
          if (uVar5 == 0) break;
          if (uVar5 == 0x26) {
            uVar9 = uVar4;
            if (uVar4 == 0) break;
LAB_10820ab0:
            uVar5 = 0;
            uVar4 = uVar9;
LAB_10820b30:
            if (uVar4 == 0) break;
          }
          else {
            if (uVar5 != 0x27) {
              if (uVar5 != 0x28) {
                if (0x25 < uVar5) {
                  FUN_10800394(0,0);
                }
                if ((puVar10[uVar5 + 7] & 7) != 0) {
                  uVar9 = uVar5;
                }
                goto LAB_10820ab0;
              }
              uVar6 = 0;
              do {
                if (5 < uVar6) {
                  uVar4 = 0;
                  break;
                }
                uVar4 = (uint)(ushort)(&DAT_108d146a)[uVar6];
                uVar9 = uVar4;
                if (uVar4 == 0) goto LAB_10820ab0;
                if (0x25 < uVar4) {
                  FUN_10800394(0,0);
                }
                uVar6 = uVar6 + 1;
              } while ((puVar10[uVar4 + 7] & 7) == 0);
              if (uVar4 != 0) goto LAB_10820b30;
              break;
            }
            if (uVar4 != 0) {
              uVar5 = 6;
              goto LAB_10820b30;
            }
            if ((puVar10[0xd] & 7) == 0) goto LAB_10820ab0;
            uVar4 = 6;
            uVar5 = 0;
          }
          FUN_108695c0(local_30,puVar10,uVar4,&local_3c);
          if (*(char *)((int)param_2 + 0x12) == '\0') {
            iVar8 = FUN_10882188(uVar4,local_3c,puVar10,local_28,puVar3);
            if (iVar8 != 0) {
              FUN_1086bbe8(local_30,puVar3);
              *puVar3 = 0;
              *(undefined4 *)(iVar2 + 0x4340) = 0;
              *(undefined4 *)(iVar2 + 0x4344) = 0;
              *(undefined4 *)(iVar2 + 0x4348) = 0;
              *(undefined4 *)(iVar2 + 0x434c) = 0;
              *(undefined4 *)(iVar2 + 0x4350) = 0;
              *(undefined4 *)(iVar2 + 0x4354) = 0;
              *(undefined4 *)(iVar2 + 0x4358) = 0;
              *(undefined4 *)(iVar2 + 0x435c) = 0;
              *(ushort *)(iVar2 + 0x433e) = *(ushort *)(iVar2 + 0x433e) & 0xff80;
            }
          }
          else {
            FUN_1086bbe8(local_30,param_2 + 5);
          }
          iVar8 = local_30;
          puVar1 = local_38;
          if ((uVar4 == 0x13) && (*(short *)((int)puVar10 + 0x12) != 0)) {
            uVar12 = FUN_1086b284();
            FUN_1086bb84();
            FUN_1086bac0(2,*(undefined1 *)((int)puVar10 + 0x19));
            FUN_1086bac0(0x2f,7);
            FUN_1086b9a4(iVar8,0x9c);
            FUN_1086b880();
            *puVar1 = uVar12;
          }
          uVar12 = 1;
        }
      }
      uVar11 = uVar11 + 1;
    } while (uVar11 < *(byte *)((int)param_2 + 0x11));
  }
  if ((*param_2 & 0x800000) != 0) {
    FUN_1086c22c(*(undefined4 *)(local_30 + 0xc),0x67,*(undefined1 *)(iVar7 + 0x19));
  }
  return uVar12;
}

