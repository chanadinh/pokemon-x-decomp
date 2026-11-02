/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10824414(int param_1,uint *param_2)

{
  byte bVar1;
  char cVar2;
  bool bVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  byte local_34 [8];
  int local_2c [2];
  
  uVar4 = FUN_10873164(*(undefined4 *)(param_1 + 4),*(undefined4 *)(param_1 + 8),
                       (*param_2 & 0x1fff) >> 8);
  uVar4 = FUN_108672c0(*(undefined4 *)(param_1 + 4),uVar4 | 0x800,local_2c);
  uVar7 = 0;
  uVar8 = 0;
  if (uVar4 != 0) {
    do {
      iVar5 = FUN_108707d4(*(undefined4 *)(param_1 + 8),*(undefined1 *)((int)local_2c + uVar7));
      if ((iVar5 != 0) && (iVar6 = FUN_10874420(iVar5,0xd), iVar6 != 0)) {
        local_34[uVar8] = *(byte *)(iVar5 + 0x19);
        uVar8 = uVar8 + 1 & 0xff;
      }
      uVar7 = uVar7 + 1 & 0xff;
    } while (uVar7 < uVar4);
  }
  uVar4 = 0;
  if (uVar8 != 0) {
    local_2c[0] = param_1 + 0x1000;
    do {
      bVar1 = local_34[uVar4];
      iVar5 = *(int *)(param_1 + 8);
      if (0x17 < bVar1) {
        FUN_10800394(0,0);
      }
      iVar5 = iVar5 + (uint)bVar1 * 4;
      if (*(int *)(iVar5 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      iVar5 = *(int *)(iVar5 + 0x84);
      bVar3 = false;
      if ((*(byte *)(iVar5 + 0x169) & 8) != 0) {
        uVar7 = *(uint *)(iVar5 + 0xa0);
        cVar2 = *(char *)(iVar5 + 0x19);
        *(byte *)(iVar5 + 0x169) = *(byte *)(iVar5 + 0x169) & 0xf7;
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x1c,*(undefined1 *)(iVar5 + 0x19),3);
        if ((*(uint *)(iVar5 + 0x84) & 7) != 0) {
          FUN_108695c0(param_1,iVar5,0x1a,0);
        }
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x38,cVar2,0);
        if ((uVar7 & 7) == 0) {
          uVar7 = 0;
          if (*(byte *)(local_2c[0] + 0x8c) != 0) {
            do {
              if (((*(uint *)(param_1 + uVar7 * 0x10 + 0x10c0) & 0xf) != 8) &&
                 (*(char *)(*(int *)(param_1 + uVar7 * 0x10 + 0x10bc) + 0x19) == cVar2)) {
                iVar6 = param_1 + uVar7 * 0x10;
                if (iVar6 != -0x10bc) {
                  *(undefined1 *)(iVar6 + 0x10c9) = 1;
                }
                break;
              }
              uVar7 = uVar7 + 1;
            } while (uVar7 < *(byte *)(local_2c[0] + 0x8c));
          }
        }
        if ((*(char *)(iVar5 + 0x171) != '\0') && ((byte)(*(char *)(iVar5 + 0x171) - 1U) < 0x18)) {
          FUN_1086844c(param_1,iVar5,0,1);
        }
        bVar3 = true;
      }
      iVar6 = FUN_108651d8(param_1,iVar5,1);
      if (iVar6 != 0) {
        bVar3 = true;
      }
      if ((*(uint *)(iVar5 + 0x94) & 7) != 0) {
        FUN_108695c0(param_1,iVar5,0x1e,0);
        bVar3 = true;
      }
      if ((*(uint *)(iVar5 + 0x9c) & 7) == 0) {
        if (bVar3) goto LAB_108246b4;
      }
      else {
        FUN_108695c0(param_1,iVar5,0x20,0);
LAB_108246b4:
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x441,local_34[uVar4],0xffff0000);
      }
      uVar4 = uVar4 + 1 & 0xff;
    } while (uVar4 < uVar8);
  }
  return 1;
}

