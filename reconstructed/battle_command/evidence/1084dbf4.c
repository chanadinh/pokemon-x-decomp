/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

/* WARNING: Type propagation algorithm not settling */

void FUN_1084dbf4(uint *param_1,undefined4 param_2,int param_3,uint *param_4)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  uint uVar4;
  uint uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  
  FUN_10862078();
  if (param_3 == 0x22) {
    uVar5 = *param_4;
    if (5999 < *param_1) {
      thunk_FUN_0011e864(0,0);
    }
    uVar4 = *param_1;
    *param_1 = uVar4 + 1;
    *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
    FUN_1084db70(param_1,param_4[1]);
    return;
  }
  if (param_3 < 0x22) {
    if (param_3 == 9) {
      iVar9 = 0;
      do {
        uVar5 = param_4[iVar9];
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        iVar9 = iVar9 + 1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
      } while (iVar9 < 9);
      return;
    }
    if (param_3 < 9) {
      switch(param_3) {
      default:
        goto switchD_1084dc3c_caseD_0;
      case 1:
        uVar5 = *param_1;
        uVar1 = (undefined1)*param_4;
        break;
      case 2:
        uVar5 = *param_4;
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        uVar5 = *param_1;
        uVar1 = (undefined1)param_4[1];
        break;
      case 3:
        uVar1 = FUN_1084dab0(*param_4,param_4[1],5,3);
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar5 = *param_1;
        *param_1 = uVar5 + 1;
        *(undefined1 *)((int)param_1 + uVar5 + 8) = uVar1;
        uVar5 = *param_1;
        uVar1 = (undefined1)param_4[2];
        break;
      case 4:
        uVar1 = FUN_1084dab0(*param_4,param_4[1],5,3);
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar5 = *param_1;
        *param_1 = uVar5 + 1;
        *(undefined1 *)((int)param_1 + uVar5 + 8) = uVar1;
        uVar5 = param_4[2];
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        FUN_10862078(param_1,param_4[3] & 0xffff);
        return;
      case 5:
        uVar2 = FUN_1084d9a0(2,*param_4,param_4[1],param_4[2],5,5,5);
        FUN_10862078(param_1,uVar2);
        FUN_10862078(param_1,param_4[3] & 0xffff);
        FUN_10862078(param_1,param_4[4] & 0xffff);
        return;
      case 6:
        uVar2 = FUN_1084d9a0(2,*param_4,param_4[1],param_4[2],5,5,5);
        uVar3 = FUN_1084d9a0(2,param_4[3],param_4[4],param_4[5],5,5,5);
        FUN_10862078(param_1,uVar2);
        FUN_10862078(param_1,uVar3);
        return;
      case 7:
        iVar9 = 0;
        do {
          uVar5 = param_4[iVar9];
          if (5999 < *param_1) {
            thunk_FUN_0011e864(0,0);
          }
          uVar4 = *param_1;
          iVar9 = iVar9 + 1;
          *param_1 = uVar4 + 1;
          *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        } while (iVar9 < 7);
        return;
      case 8:
        iVar9 = 0;
        do {
          uVar5 = param_4[iVar9];
          if (5999 < *param_1) {
            thunk_FUN_0011e864(0,0);
          }
          uVar4 = *param_1;
          iVar9 = iVar9 + 1;
          *param_1 = uVar4 + 1;
          *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        } while (iVar9 < 8);
        return;
      }
joined_r0x1084e110:
      if (5999 < uVar5) {
        thunk_FUN_0011e864(0,0);
      }
      uVar5 = *param_1;
      *param_1 = uVar5 + 1;
      *(undefined1 *)((int)param_1 + uVar5 + 8) = uVar1;
      return;
    }
    if (param_3 == 0x14) {
      uVar1 = FUN_1084dab0(*param_4,param_4[1],5,3);
      if (5999 < *param_1) {
        thunk_FUN_0011e864(0,0);
      }
      uVar5 = *param_1;
      *param_1 = uVar5 + 1;
      *(undefined1 *)((int)param_1 + uVar5 + 8) = uVar1;
      uVar1 = FUN_1084dab0(param_4[2],param_4[3],5,3);
      uVar5 = *param_1;
      goto joined_r0x1084e110;
    }
    if (param_3 < 0x14) {
      if (param_3 == 0x11) {
        FUN_10862078(param_1,*param_4 & 0xffff);
        return;
      }
      if (param_3 == 0x12) {
        uVar5 = *param_4;
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        FUN_10862078(param_1,param_4[1] & 0xffff);
        return;
      }
      if (param_3 == 0x13) {
        uVar1 = FUN_1084dab0(*param_4,param_4[1],5,3);
        uVar5 = *param_1;
        goto joined_r0x1084e00c;
      }
    }
    else {
      if (param_3 == 0x15) {
        uVar6 = FUN_1084dab0(*param_4,param_4[1],5,3);
        uVar7 = FUN_1084dab0(param_4[2],param_4[3],7,1);
        FUN_10862078(param_1,uVar6);
        FUN_10862078(param_1,uVar7);
        FUN_10862078(param_1,param_4[4] & 0xffff);
        return;
      }
      if (param_3 == 0x16) {
        uVar4 = *param_4;
        uVar10 = param_4[1];
        uVar11 = param_4[2];
        uVar12 = param_4[3];
        uVar5 = param_4[4];
        if (7 < (int)uVar4) {
          FUN_10800394(0,0);
        }
        if (3 < (int)uVar10) {
          FUN_10800394(0,0);
        }
        if (1 < (int)uVar11) {
          FUN_10800394(0,0);
        }
        if (1 < (int)uVar12) {
          FUN_10800394(0,0);
        }
        if (1 < (int)uVar5) {
          FUN_10800394(0,0);
        }
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar8 = *param_1;
        *param_1 = uVar8 + 1;
        *(byte *)((int)param_1 + uVar8 + 8) =
             (byte)((uVar4 << 0x1d) >> 0x18) | (byte)((uVar10 << 0x1e) >> 0x1b) |
             (byte)((uVar11 << 0x1f) >> 0x1d) | (byte)((uVar12 << 0x1f) >> 0x1e) | (byte)uVar5 & 1;
        FUN_10862078(param_1,param_4[5] & 0xffff);
        return;
      }
      if (param_3 == 0x21) {
        FUN_1084db70(param_1,*param_4);
        return;
      }
    }
  }
  else {
    if (param_3 == 0x34) {
      uVar6 = FUN_1084d9a0(3,*param_4,param_4[1],param_4[2],5,5,0xe);
      FUN_1084d930(param_1,uVar6);
      FUN_10862078(param_1,param_4[3] & 0xffff);
      return;
    }
    if (param_3 < 0x35) {
      if (param_3 == 0x26) {
        uVar5 = *param_4;
        uVar4 = param_4[1];
        uVar10 = param_4[2];
        uVar11 = param_4[3];
        if (0x1f < (int)uVar5) {
          FUN_10800394(0,0);
        }
        if (0x1f < (int)uVar4) {
          FUN_10800394(0,0);
        }
        if (0x1f < (int)uVar10) {
          FUN_10800394(0,0);
        }
        if (0x1f < (int)uVar11) {
          FUN_10800394(0,0);
        }
        FUN_1084d930(param_1,(uVar4 & 0x1f) << 10 | (uVar5 & 0x1f) << 0xf | (uVar10 & 0x1f) << 5 |
                             uVar11 & 0x1f);
        FUN_10862078(param_1,param_4[4] & 0xffff);
        FUN_10862078(param_1,param_4[5] & 0xffff);
        return;
      }
      if (param_3 < 0x27) {
        if (param_3 == 0x23) {
          uVar2 = FUN_1084d9a0(2,*param_4,param_4[1],param_4[2],5,5,5);
          FUN_10862078(param_1,uVar2);
          return;
        }
        if (param_3 == 0x24) {
          uVar6 = FUN_1084d9a0(3,*param_4,param_4[1],param_4[2],5,5,0xe);
          FUN_1084d930(param_1,uVar6);
          uVar5 = *param_1;
          uVar1 = (undefined1)param_4[3];
          goto joined_r0x1084e110;
        }
        if (param_3 == 0x25) {
          uVar11 = *param_4;
          uVar5 = param_4[1];
          uVar4 = param_4[2];
          uVar10 = param_4[3];
          if (7 < (int)uVar11) {
            FUN_10800394(0,0);
          }
          if (7 < (int)uVar5) {
            FUN_10800394(0,0);
          }
          if (1 < (int)uVar4) {
            FUN_10800394(0,0);
          }
          if (1 < (int)uVar10) {
            FUN_10800394(0,0);
          }
          if (5999 < *param_1) {
            thunk_FUN_0011e864(0,0);
          }
          uVar12 = *param_1;
          *param_1 = uVar12 + 1;
          *(byte *)((int)param_1 + uVar12 + 8) =
               (byte)((uVar11 << 0x1d) >> 0x18) | (byte)((uVar5 << 0x1d) >> 0x1b) |
               (byte)((uVar4 << 0x1f) >> 0x1e) | (byte)uVar10 & 1;
          FUN_10862078(param_1,param_4[4] & 0xffff);
          return;
        }
      }
      else {
        if (param_3 == 0x30) {
          return;
        }
        if (param_3 == 0x32) {
          uVar1 = FUN_1084dab0(*param_4,param_4[1],4);
          uVar5 = *param_1;
          goto joined_r0x1084e110;
        }
        if (param_3 == 0x33) {
          uVar6 = FUN_1084d9a0(3,*param_4,param_4[1],param_4[2],5,5,0xe);
          FUN_1084d930(param_1,uVar6);
          return;
        }
      }
    }
    else {
      if (param_3 == 0x43) {
        uVar5 = *param_4;
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        uVar5 = *param_1;
        uVar1 = (undefined1)param_4[1];
joined_r0x1084e00c:
        if (5999 < uVar5) {
          thunk_FUN_0011e864(0,0);
        }
        uVar5 = *param_1;
        *param_1 = uVar5 + 1;
        *(undefined1 *)((int)param_1 + uVar5 + 8) = uVar1;
        FUN_10862078(param_1,param_4[2] & 0xffff);
        return;
      }
      if (param_3 < 0x43) {
        if (param_3 == 0x35) {
          uVar6 = FUN_1084d9a0(3,*param_4,param_4[1],param_4[2],5,5,0xe);
          uVar1 = FUN_1084dab0(param_4[3],param_4[4],7,1);
          FUN_1084d930(param_1,uVar6);
          uVar5 = *param_1;
        }
        else {
          if (param_3 == 0x36) {
            uVar2 = FUN_1084d9a0(2,*param_4,param_4[1],param_4[2],5,5,6);
            FUN_10862078(param_1,uVar2);
            FUN_10862078(param_1,param_4[3] & 0xffff);
            FUN_10862078(param_1,param_4[4] & 0xffff);
            FUN_10862078(param_1,param_4[5] & 0xffff);
            return;
          }
          if (param_3 != 0x42) goto switchD_1084dc3c_caseD_0;
          uVar1 = FUN_1084dab0(*param_4,param_4[1],5,3);
          uVar5 = *param_1;
        }
        goto joined_r0x1084e110;
      }
      if (param_3 == 0x44) {
        uVar2 = FUN_1084d9a0(2,*param_4,param_4[1],param_4[2],5,5,6);
        FUN_10862078(param_1,uVar2);
        FUN_10862078(param_1,param_4[3] & 0xffff);
        return;
      }
      if (param_3 == 0x53) {
        uVar5 = *param_4;
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        uVar5 = param_4[1];
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        FUN_1084db70(param_1,param_4[2]);
        return;
      }
      if (param_3 == 0x54) {
        uVar5 = *param_4;
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        uVar5 = param_4[1];
        if (5999 < *param_1) {
          thunk_FUN_0011e864(0,0);
        }
        uVar4 = *param_1;
        *param_1 = uVar4 + 1;
        *(char *)((int)param_1 + uVar4 + 8) = (char)uVar5;
        FUN_10862078(param_1,param_4[2] & 0xffff);
        FUN_10862078(param_1,param_4[3] & 0xffff);
        return;
      }
    }
  }
switchD_1084dc3c_caseD_0:
  FUN_10800394(0,0);
  return;
}

