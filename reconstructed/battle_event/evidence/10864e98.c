/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10864e98(undefined4 param_1,uint param_2,uint param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  uint extraout_r1;
  uint extraout_r1_00;
  int *piVar3;
  uint uVar4;
  int iVar5;
  int *piVar6;
  undefined8 uVar7;
  uint uVar8;
  uint uVar9;
  
  uVar9 = param_3;
  uVar2 = param_2;
  uVar8 = param_2;
  piVar6 = DAT_108fca60;
joined_r0x10864eb0:
  if (piVar6 == (int *)0x0) {
    return;
  }
LAB_10864eb8:
  piVar3 = (int *)piVar6[1];
  if (piVar3 != (int *)0x0) {
    piVar3[6] = piVar3[6] | 0x80000000;
  }
  uVar1 = piVar6[6];
  if ((((uVar1 & 0x1000000) == 0) || ((uVar1 & 0x10000000) != 0)) &&
     ((uVar1 & 0x2000000) == 0 && (uVar1 & 0x40000000) == 0)) {
    if (param_3 != 7) {
      uVar2 = (uint)*(byte *)(piVar6 + 4);
    }
    if ((((param_3 == 7 || uVar2 == param_3) &&
         ((uVar2 = 0, (uVar1 & 0xffff) == 0 ||
          (uVar2 = DAT_108fca68, (uVar1 & 0xffff) < DAT_108fca68)))) && ((uVar1 & 0x20000000) != 0))
       && ((param_2 != 0x7c || ((uVar1 & 0x4000000) != 0)))) {
      iVar5 = piVar6[2];
      uVar4 = 0;
      if ((uVar1 & 0xffffff) >> 0x10 != 0) {
        do {
          uVar2 = (uint)*(byte *)(iVar5 + uVar4 * 8);
          if (uVar2 == param_2) {
            if (param_4 != 0) {
              uVar7 = FUN_108c59b4(param_1,piVar6,param_2);
              uVar2 = (uint)((ulonglong)uVar7 >> 0x20);
              if ((int)uVar7 != 0) break;
            }
            piVar6[6] = piVar6[6] | 0x1000000;
            (**(code **)(iVar5 + uVar4 * 8 + 4))
                      (piVar6,param_1,*(undefined1 *)((int)piVar6 + 0x3a),piVar6 + 7,param_1,uVar8,
                       uVar9);
            uVar1 = piVar6[6];
            if ((uVar1 & 0x10000000) == 0) {
              uVar1 = uVar1 & 0xfeffffff;
            }
            else {
              uVar1 = uVar1 & 0xefffffff;
            }
            piVar6[6] = uVar1;
            uVar2 = extraout_r1;
            if ((uVar1 & 0x8000000) != 0) {
              if (piVar3 == (int *)0x0) goto LAB_10864ff8;
              if ((piVar3[6] & 0x20000000U) != 0) goto LAB_10864fec;
              piVar3 = (int *)piVar6[1];
              goto LAB_10864fd8;
            }
            break;
          }
          uVar4 = uVar4 + 1;
        } while (uVar4 < (uVar1 & 0xffffff) >> 0x10);
      }
    }
  }
  goto LAB_108650d4;
  while ((piVar3[6] & 0x20000000U) == 0) {
LAB_10864fd8:
    if (piVar3 == (int *)0x0) goto LAB_10864ff8;
  }
LAB_10864fec:
  piVar3[6] = piVar3[6] | 0x80000000;
LAB_10864ff8:
  if ((piVar6 != (int *)0x0) && (uVar1 = piVar6[6], (uVar1 & 0x20000000) != 0)) {
    if ((uVar1 & 0x1000000) == 0) {
      if (piVar6 == DAT_108fca60) {
        DAT_108fca60 = (int *)piVar6[1];
      }
      if (*piVar6 != 0) {
        *(int *)(*piVar6 + 4) = piVar6[1];
      }
      if ((int *)piVar6[1] != (int *)0x0) {
        *(int *)piVar6[1] = *piVar6;
      }
      if (DAT_108fca64 == 0) {
        FUN_10800394(0,0);
        uVar2 = extraout_r1_00;
      }
      else {
        *piVar6 = 0;
        piVar6[1] = 0;
        piVar6[2] = 0;
        piVar6[3] = 0;
        piVar6[4] = 0;
        piVar6[5] = 0;
        piVar6[6] = 0;
        piVar6[7] = 0;
        piVar6[8] = 0;
        piVar6[9] = 0;
        piVar6[10] = 0;
        piVar6[0xb] = 0;
        piVar6[0xc] = 0;
        piVar6[0xd] = 0;
        uVar2 = DAT_108fca64 - 1;
        piVar6[0xe] = 0;
        DAT_108fca64 = uVar2;
        (&DAT_10b1196c)[uVar2] = piVar6;
      }
    }
    else {
      uVar2 = uVar1 | 0x8000000;
      piVar6[6] = uVar2;
    }
  }
LAB_108650d4:
  if (piVar3 == (int *)0x0) {
    return;
  }
  uVar1 = piVar3[6];
  if ((uVar1 & 0x20000000) == 0 || (uVar1 & 0x80000000) == 0) goto LAB_108650fc;
  piVar3[6] = uVar1 & 0x7fffffff;
  piVar6 = piVar3;
  goto LAB_10864eb8;
LAB_108650fc:
  piVar6 = (int *)piVar6[1];
  goto joined_r0x10864eb0;
}

