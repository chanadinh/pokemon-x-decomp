/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_10823014(int param_1,uint *param_2,undefined4 param_3)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined4 uVar5;
  int iVar6;
  uint *puVar7;
  int iVar8;
  undefined4 uVar9;
  undefined4 local_3c;
  
  iVar8 = 0;
  if ((*param_2 & 0x1fff) >> 8 != 0x1f) {
    iVar8 = *(int *)(param_1 + 8);
    uVar3 = (*param_2 & 0x1fff) >> 8;
    if (0x17 < uVar3) {
      FUN_10800394(0,0);
    }
    iVar8 = iVar8 + uVar3 * 4;
    if (*(int *)(iVar8 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar8 = *(int *)(iVar8 + 0x84);
  }
  bVar1 = false;
  uVar3 = 0;
  local_3c = 0;
  if (*(char *)((int)param_2 + 0x16) != '\0') {
    do {
      iVar2 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)((int)param_2 + uVar3 + 0xc));
      if (iVar2 != 10) {
        uVar4 = (uint)*(byte *)((int)param_2 + uVar3 + 0xc);
        iVar2 = *(int *)(param_1 + 8);
        if (0x17 < uVar4) {
          FUN_10800394(0,0);
        }
        iVar2 = iVar2 + uVar4 * 4;
        if (*(int *)(iVar2 + 0x84) == 0) {
          FUN_10800394(0,0);
        }
        uVar5 = *(undefined4 *)(iVar2 + 0x84);
        iVar2 = FUN_10874420(uVar5,0xd);
        if ((iVar2 != 0) &&
           (iVar2 = FUN_108722e8(uVar5,(char)param_2[1],(int)*(char *)((int)param_2 + 0x17)),
           iVar2 != 0)) {
          bVar1 = true;
          if ((*param_2 & 0x800000) != 0 && iVar8 != 0) {
            FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar8 + 0x19));
          }
          break;
        }
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < *(byte *)((int)param_2 + 0x16));
  }
  uVar3 = 0;
  if (*(char *)((int)param_2 + 0x16) != '\0') {
    do {
      iVar2 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)((int)param_2 + uVar3 + 0xc));
      if (iVar2 != 10) {
        uVar4 = (uint)*(byte *)((int)param_2 + uVar3 + 0xc);
        iVar2 = *(int *)(param_1 + 8);
        if (0x17 < uVar4) {
          FUN_10800394(0,0);
        }
        iVar2 = iVar2 + uVar4 * 4;
        if (*(int *)(iVar2 + 0x84) == 0) {
          FUN_10800394(0,0);
        }
        iVar6 = *(int *)(iVar2 + 0x84);
        iVar2 = FUN_10874420(iVar6,0xd);
        if (iVar2 != 0) {
          puVar7 = (uint *)0x0;
          if (((byte)param_2[6] & 8) != 0) {
            puVar7 = param_2 + 7;
          }
          uVar4 = ((byte)param_2[6] & 0x10) >> 4;
          if ((uVar4 == 0) && ((*param_2 & 0x1fff) >> 8 != 0x1f)) {
            uVar4 = (*param_2 & 0x1fff) >> 8;
            iVar2 = *(int *)(param_1 + 8);
            if (0x17 < uVar4) {
              FUN_10800394(0,0);
            }
            iVar2 = iVar2 + uVar4 * 4;
            if (*(int *)(iVar2 + 0x84) == 0) {
              FUN_10800394(0,0);
            }
            iVar2 = *(int *)(iVar2 + 0x84);
            uVar9 = 0;
            uVar5 = thunk_FUN_0039df44(0,0xd);
            FUN_1086bb84();
            FUN_1086bac0(3,*(undefined1 *)(iVar2 + 0x19));
            FUN_1086bac0(4,*(undefined1 *)(iVar6 + 0x19));
            FUN_1086bac0(0x12,uVar9);
            FUN_1086b9fc(0x53,uVar5);
            FUN_1086b9a4(param_1,5);
            iVar2 = FUN_1086b91c(0x53);
            uVar4 = (uint)(iVar2 != 0);
            FUN_1086b880();
          }
          iVar2 = FUN_10860e74(param_1,(*param_2 & 0x1fff) >> 8,iVar6,(char)param_2[1],
                               (int)*(char *)((int)param_2 + 0x17),0x1f,param_3,param_2[2],
                               ((byte)param_2[6] & 2) >> 1,~(uint)(byte)param_2[6] & 1,puVar7,uVar4)
          ;
          if (iVar2 != 0) {
            if ((param_2[6] & 8) == 0) {
              FUN_1086bbe8(param_1,param_2 + 7);
            }
            local_3c = 1;
          }
        }
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < *(byte *)((int)param_2 + 0x16));
  }
  if ((bVar1) && ((*param_2 & 0x800000) != 0 && iVar8 != 0)) {
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar8 + 0x19));
  }
  return local_3c;
}

