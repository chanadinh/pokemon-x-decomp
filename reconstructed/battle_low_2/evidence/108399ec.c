/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108399ec(int *param_1)

{
  byte bVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int iVar5;
  byte *pbVar6;
  int *piVar7;
  
  if (DAT_108fcb84 != 0x11fd) {
    DAT_108fcb84 = 0x11fd;
  }
  if (99 < param_1[0x1ae]) {
    iVar2 = param_1[0x1ae];
    piVar4 = param_1 + 0x1ae;
    pbVar6 = (byte *)(param_1 + 0x1b2);
    if (iVar2 == 0x67) {
      iVar2 = thunk_FUN_00442280(*(undefined4 *)(param_1[*(byte *)(param_1 + 0x1b2) + 0x40] + 300));
      if (iVar2 != 0) {
        FUN_1085e148(param_1[*(byte *)(param_1 + 0x1b2) + 0x40]);
        (**(code **)(*(int *)param_1[*pbVar6 + 0x40] + 0xfc))((int *)param_1[*pbVar6 + 0x40],1,0);
        (**(code **)(*(int *)param_1[*pbVar6 + 0x40] + 0x100))
                  ((int *)param_1[*pbVar6 + 0x40],(int)*(char *)((int)param_1 + 0x727));
        (**(code **)(*(int *)param_1[*pbVar6 + 0x40] + 0x104))
                  ((int *)param_1[*pbVar6 + 0x40],(int)(char)param_1[0x1c9]);
        FUN_108594ec(param_1);
        FUN_108596c4(param_1,0);
        *piVar4 = param_1[0x1ae] + 1;
      }
    }
    else if (iVar2 < 0x67) {
      if (iVar2 == 100) {
        *(undefined1 *)((int)param_1 + 0x727) =
             *(undefined1 *)(param_1[*(byte *)(param_1 + 0x1b2) + 0x40] + 0x15a);
        (**(code **)(*(int *)param_1[*(byte *)(param_1 + 0x1b2) + 0x40] + 0xc4))();
        *piVar4 = param_1[0x1ae] + 1;
      }
      else if (iVar2 == 0x65) {
        if ((char)param_1[0x25] != '\0') {
          (**(code **)(*(int *)param_1[*(byte *)(param_1 + 0x1b2) + 0x40] + 0xb4))();
          *piVar4 = param_1[0x1ae] + 1;
        }
      }
      else if (iVar2 == 0x66) {
        piVar7 = (int *)param_1[*(byte *)(param_1 + 0x1b2) + 0x40];
        uVar3 = (**(code **)(*param_1 + 0x20))(param_1,*(undefined1 *)((int)param_1 + 0x6c9));
        (**(code **)(*piVar7 + 0xdc))(piVar7,uVar3);
        *piVar4 = param_1[0x1ae] + 1;
      }
    }
    else {
      if (iVar2 == 0x68) {
        return 1;
      }
      if (iVar2 == 0x78) {
        iVar5 = param_1[*(byte *)(param_1 + 0x1b2) + 0x40];
        iVar2 = (**(code **)(**(int **)(iVar5 + 0x68) + 0x20))
                          (*(int **)(iVar5 + 0x68),*(uint *)(iVar5 + 0x6c) & 0xff);
        if (*(short *)(iVar5 + 0x138) == 0x2a9) {
          bVar1 = *(byte *)(iVar2 + 0x153);
          if (*(ushort *)(iVar5 + 0x13a) != (ushort)bVar1) {
            *(ushort *)(iVar5 + 0x13a) = (ushort)bVar1;
            thunk_FUN_004464ec(*(undefined4 *)(iVar5 + 300),(ushort)bVar1);
          }
        }
        param_1[0x1ae] = 0x68;
      }
    }
    return 0;
  }
  switch(param_1[0x1ae]) {
  case 0:
    if ((char)param_1[0x1c9] == '\0') goto LAB_108583b0;
    iVar2 = 1;
    break;
  case 1:
    FUN_10859464(param_1,0x29f);
    iVar2 = 3;
    break;
  default:
    goto switchD_1085833c_caseD_2;
  case 3:
    iVar2 = FUN_10858cb8(param_1,0);
    if (iVar2 == 0) {
      return 0;
    }
LAB_108583b0:
    FUN_10859464(param_1,param_1[0x1b8]);
    iVar2 = 5;
    break;
  case 5:
    iVar2 = FUN_10858cb8(param_1,0);
    if (iVar2 == 0) {
      return 0;
    }
    if ((char)param_1[0x1c9] == '\0') goto LAB_108583f0;
    iVar2 = 6;
    break;
  case 6:
    FUN_10859464(param_1,0x2a0);
    iVar2 = 8;
    break;
  case 8:
    iVar2 = FUN_10858cb8(param_1,0);
    if (iVar2 == 0) {
      return 0;
    }
LAB_108583f0:
    iVar2 = 9;
    break;
  case 9:
    return 1;
  }
  param_1[0x1ae] = iVar2;
switchD_1085833c_caseD_2:
  return 0;
}

