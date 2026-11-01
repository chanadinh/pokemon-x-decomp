/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_10819600(int param_1,uint *param_2)

{
  char cVar1;
  char cVar2;
  undefined1 uVar3;
  byte bVar4;
  undefined2 uVar5;
  short sVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  bool bVar15;
  undefined1 auStack_28 [4];
  
  uVar7 = *(uint *)(param_1 + 0x437c);
  if ((uVar7 & 0x40000000) == 0) {
    uVar12 = 1;
  }
  else {
    uVar12 = (uVar7 & 0x10000000) >> 0x1c;
  }
  uVar8 = *param_2;
  uVar7 = uVar7 & 0x3ff;
  if (((uVar8 & 0x1000000) != 0) && (uVar12 == 0)) {
    return;
  }
  if ((uVar8 & 0x2000000) != 0) {
    iVar13 = *(int *)(param_1 + 8);
    uVar8 = (uVar8 & 0x1fff) >> 8;
    if (0x17 < uVar8) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + uVar8 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = FUN_10874420(*(undefined4 *)(iVar13 + 0x84),0xd);
    if (iVar13 == 0) {
      return;
    }
  }
  switch(*param_2 & 0xff) {
  case 0:
    iVar13 = *(int *)(param_1 + 8);
    uVar7 = (*param_2 & 0x1fff) >> 8;
    if (0x17 < uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar10 = *(undefined4 *)(iVar13 + 0x84);
    iVar13 = FUN_10874420(uVar10,0xd);
    if ((iVar13 != 0) || ((param_2[1] & 2) != 0)) {
      if ((param_2[1] & 1) != 0) {
        iVar13 = FUN_10874420(uVar10,0xd);
        iVar9 = FUN_10874420(uVar10,0xe);
        if (iVar13 == iVar9) goto LAB_1081ad30;
      }
      iVar13 = FUN_1086c06c(param_1,uVar10);
      if (iVar13 != 0) goto LAB_1081a994;
    }
    goto LAB_1081ac70;
  case 1:
    iVar13 = FUN_1086c2cc(param_1 + 0x120c,(*param_2 & 0x1fff) >> 8);
    if (iVar13 != 10) {
      iVar13 = *(int *)(param_1 + 8);
      uVar7 = (*param_2 & 0x1fff) >> 8;
      if (0x17 < uVar7) {
        FUN_10800394(0,0);
      }
      if (*(int *)(iVar13 + uVar7 * 4 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      uVar11 = 0x66;
      uVar10 = *(undefined4 *)(param_1 + 0xc);
      goto LAB_10819890;
    }
    goto LAB_1081ac70;
  case 2:
    iVar13 = *(int *)(param_1 + 8);
    uVar7 = (*param_2 & 0x1fff) >> 8;
    if (0x17 < uVar7) {
      FUN_10800394(0,0);
    }
    if (*(int *)(iVar13 + uVar7 * 4 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar10 = *(undefined4 *)(param_1 + 0xc);
LAB_1081988c:
    uVar11 = 0x67;
    goto LAB_10819890;
  case 3:
    iVar13 = 0;
    if ((*param_2 & 0x1fff) >> 8 != 0x1f) {
      iVar13 = *(int *)(param_1 + 8);
      uVar7 = (*param_2 & 0x1fff) >> 8;
      if (0x17 < uVar7) {
        FUN_10800394(0,0);
      }
      iVar13 = iVar13 + uVar7 * 4;
      if (*(int *)(iVar13 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      iVar13 = *(int *)(iVar13 + 0x84);
    }
    if ((*param_2 & 0x800000) != 0 && iVar13 != 0) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar13 + 0x19));
    }
    FUN_1086bbe8(param_1,param_2 + 1);
    if ((*param_2 & 0x800000) != 0 && iVar13 != 0) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x67,*(undefined1 *)(iVar13 + 0x19));
    }
    if ((*(ushort *)((int)param_2 + 6) & 0x8000) != 0) {
      *(byte *)(param_1 + 0x1093) = *(byte *)(param_1 + 0x1093) | 0x10;
    }
    goto LAB_1081ad08;
  case 4:
    uVar12 = FUN_10821948(param_1,param_2,uVar7);
    break;
  case 5:
    uVar12 = FUN_1081d558(param_1,param_2);
    break;
  case 6:
    uVar12 = FUN_1081dd1c(param_1,param_2);
    break;
  case 7:
    uVar12 = FUN_1081f7a0(param_1,param_2);
    break;
  case 8:
    uVar12 = FUN_10821b80(param_1,param_2,uVar7);
    break;
  case 9:
    bVar4 = *(byte *)((int)param_2 + 5);
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < bVar4) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)bVar4 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar9 = *(int *)(iVar13 + 0x84);
    iVar13 = FUN_10874420(iVar9,0xd);
    if ((iVar13 == 0) && ((*(byte *)((int)param_2 + 7) & 2) == 0)) goto LAB_1081a9ac;
    iVar13 = FUN_1086bf44(param_1,iVar9,*(undefined1 *)((int)param_2 + 6),(char)param_2[1]);
    if (iVar13 != 0) {
      FUN_1086bbe8(param_1,param_2 + 2);
      uVar3 = *(undefined1 *)((int)param_2 + 6);
      FUN_1086bb84();
      FUN_1086bac0(2,*(undefined1 *)(iVar9 + 0x19));
      FUN_1086bac0(0x18,uVar3);
      FUN_1086b9fc(0x53,0);
      FUN_1086b9a4(param_1,0x57);
      iVar13 = FUN_1086b91c(0x53);
      if (iVar13 == 0) {
        FUN_1086b880();
      }
      else {
        FUN_1086b880();
        FUN_1086c06c(param_1,iVar9);
      }
    }
    goto LAB_1081ad08;
  case 10:
    uVar12 = FUN_10820904(param_1,param_2,uVar7);
    break;
  case 0xb:
    uVar12 = FUN_1081f1a0(param_1,param_2);
    break;
  default:
    FUN_10800394(0,0);
    break;
  case 0xd:
    uVar12 = FUN_10823014(param_1,param_2,uVar7);
    break;
  case 0xe:
    uVar12 = FUN_1081f648(param_1,param_2);
    break;
  case 0xf:
    uVar12 = 0;
    uVar7 = 0;
    if ((char)param_2[1] != '\0') {
      do {
        iVar13 = *(int *)(param_1 + 8);
        uVar8 = (uint)*(byte *)((int)param_2 + uVar7 + 5);
        if (0x17 < uVar8) {
          FUN_10800394(0,0);
        }
        iVar13 = iVar13 + uVar8 * 4;
        if (*(int *)(iVar13 + 0x84) == 0) {
          FUN_10800394(0,0);
        }
        iVar9 = *(int *)(iVar13 + 0x84);
        iVar13 = FUN_10874420(iVar9,0xd);
        if (iVar13 != 0) {
          FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0xe,*(undefined1 *)((int)param_2 + uVar7 + 5))
          ;
          *(undefined1 *)(iVar9 + 0x104) = 6;
          *(undefined1 *)(iVar9 + 0x105) = 6;
          *(undefined1 *)(iVar9 + 0x106) = 6;
          *(undefined1 *)(iVar9 + 0x107) = 6;
          *(undefined1 *)(iVar9 + 0x108) = 6;
          *(undefined1 *)(iVar9 + 0x109) = 6;
          uVar12 = 1;
          *(undefined1 *)(iVar9 + 0x10a) = 6;
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 < (byte)param_2[1]);
    }
    break;
  case 0x10:
    uVar12 = FUN_10821d98(param_1,param_2);
    break;
  case 0x11:
    uVar12 = FUN_10823704(param_1,param_2);
    break;
  case 0x12:
    uVar12 = FUN_1081cf54(param_1,param_2);
    break;
  case 0x13:
    uVar12 = FUN_10822e28(param_1,param_2);
    break;
  case 0x14:
    iVar13 = FUN_1086c2cc(param_1 + 0x120c,*(undefined1 *)((int)param_2 + 5));
    if (iVar13 != 10) {
      bVar4 = *(byte *)((int)param_2 + 5);
      iVar13 = *(int *)(param_1 + 8);
      if (0x17 < bVar4) {
        FUN_10800394(0,0);
      }
      iVar13 = iVar13 + (uint)bVar4 * 4;
      if (*(int *)(iVar13 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      iVar13 = *(int *)(iVar13 + 0x84);
      iVar9 = FUN_10874420(iVar13,0xd);
      if ((iVar9 != 0) && (iVar9 = FUN_1086f960(iVar13,(char)param_2[1]), iVar9 == 0)) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x18,*(undefined1 *)((int)param_2 + 5),
                     (char)param_2[1]);
        cVar1 = *(char *)(iVar13 + 0x100);
        cVar2 = (char)param_2[1];
        bVar15 = cVar1 != cVar2;
        if (bVar15) {
          cVar1 = *(char *)(iVar13 + 0x101);
        }
        if (bVar15 && cVar1 != cVar2) {
          *(char *)(iVar13 + 0x102) = cVar2;
        }
        FUN_1086b5e0(*(undefined4 *)(param_1 + 0xc),0x6a,0x386,*(undefined1 *)((int)param_2 + 5),
                     (char)param_2[1],0xffff0000);
        goto LAB_1081ad08;
      }
    }
    goto LAB_1081ac70;
  case 0x15:
    bVar4 = *(byte *)((int)param_2 + 5);
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < bVar4) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)bVar4 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar9 = *(int *)(iVar13 + 0x84);
    iVar13 = FUN_10874420(iVar9,0xd);
    if (iVar13 != 0) {
      iVar9 = iVar9 + 0x166;
      uVar12 = 1;
      uVar7 = (uint)(byte)((byte)param_2[1] >> 3);
      *(byte *)(iVar9 + uVar7) = *(byte *)(iVar9 + uVar7) | (byte)(1 << ((byte)param_2[1] & 7));
      break;
    }
    goto LAB_1081ad30;
  case 0x16:
    bVar4 = *(byte *)((int)param_2 + 5);
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < bVar4) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)bVar4 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar9 = *(int *)(iVar13 + 0x84);
    iVar13 = FUN_10874420(iVar9,0xd);
    if (iVar13 != 0) {
      iVar9 = iVar9 + 0x166;
      uVar12 = 1;
      uVar7 = (uint)(byte)((byte)param_2[1] >> 3);
      *(byte *)(iVar9 + uVar7) = *(byte *)(iVar9 + uVar7) & ~(byte)(1 << ((byte)param_2[1] & 7));
      break;
    }
    goto LAB_1081ad30;
  case 0x17:
    bVar4 = *(byte *)((int)param_2 + 5);
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < bVar4) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)bVar4 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = *(int *)(iVar13 + 0x84);
    iVar9 = FUN_10874420(iVar13,0xd);
    if (iVar9 != 0) {
      uVar7 = (uint)(byte)((byte)param_2[1] >> 3);
      *(byte *)(iVar13 + 0x169 + uVar7) =
           *(byte *)(iVar13 + 0x169 + uVar7) | (byte)(1 << ((byte)param_2[1] & 7));
      uVar3 = *(undefined1 *)(iVar13 + 0x19);
      uVar10 = *(undefined4 *)(param_1 + 0xc);
      uVar11 = 0x1b;
LAB_1081a068:
      uVar12 = 1;
      FUN_1086c22c(uVar10,uVar11,uVar3);
      break;
    }
    goto LAB_1081ad30;
  case 0x18:
    bVar4 = *(byte *)((int)param_2 + 5);
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < bVar4) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)bVar4 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = *(int *)(iVar13 + 0x84);
    iVar9 = FUN_10874420(iVar13,0xd);
    if (iVar9 != 0) {
      uVar7 = (uint)(byte)((byte)param_2[1] >> 3);
      *(byte *)(iVar13 + 0x169 + uVar7) =
           *(byte *)(iVar13 + 0x169 + uVar7) & ~(byte)(1 << ((byte)param_2[1] & 7));
      uVar3 = *(undefined1 *)(iVar13 + 0x19);
      uVar10 = *(undefined4 *)(param_1 + 0xc);
      uVar11 = 0x1c;
      goto LAB_1081a068;
    }
    goto LAB_1081ad30;
  case 0x19:
    uVar8 = *param_2;
    uVar14 = param_2[2];
    uVar7 = param_2[1];
    uVar12 = param_2[3];
    FUN_1086bb84();
    FUN_1086bac0(2,(uVar8 & 0x1fff) >> 8);
    FUN_1086bac0(0x55,(char)uVar12);
    FUN_1086bac0(0x56,(char)uVar7);
    FUN_1086b520(0x1f,uVar14);
    FUN_1086b9a4(param_1,0xab);
    uVar10 = FUN_1086b91c(0x1f);
    FUN_1086b880();
    iVar13 = FUN_108acd0c((char)param_2[3],(char)param_2[1],uVar10);
    if (iVar13 == 0) goto LAB_1081a9ac;
    FUN_1086bbe8(param_1,param_2 + 4);
    goto LAB_1081a994;
  case 0x1a:
    uVar12 = FUN_108266b8(param_1,param_2);
    break;
  case 0x1b:
    uVar12 = 0;
    uVar7 = 0;
    do {
      uVar8 = (uVar7 >> 3) + 1 & 0xff;
      if ((uVar8 < (byte)param_2[1]) &&
         (((uint)*(byte *)((int)param_2 + uVar8 + 4) & 1 << (uVar7 & 7)) != 0)) {
        if (*(char *)((int)param_2 + 9) == '\0') {
          iVar13 = FUN_108ace70((char)param_2[2],uVar7 & 0xff);
        }
        else {
          iVar13 = FUN_108ace4c();
        }
        if (iVar13 != 0) {
          uVar12 = 1;
        }
      }
      uVar7 = uVar7 + 1;
    } while (uVar7 < 0x11);
    break;
  case 0x1c:
    uVar12 = FUN_10824fb0(param_1,param_2);
    break;
  case 0x1d:
    uVar7 = (uint)(byte)param_2[1];
    iVar13 = *(int *)(param_1 + 0x1c);
    if (uVar7 < 0xb) {
      iVar9 = iVar13 + uVar7 * 4;
      if (*(int *)(iVar9 + 0x274) != 0) {
        if (*(int *)(iVar9 + 0xc) == 0) {
          FUN_1086fd30(iVar13,uVar7);
        }
        else {
          FUN_1084d1c8();
          *(undefined4 *)(iVar9 + 0xc) = 0;
          FUN_1086fd30(iVar13,uVar7);
        }
        FUN_1086b2b4(param_1,(char)param_2[1]);
        goto LAB_1081ad08;
      }
    }
    else {
      FUN_10800394(0,0);
      FUN_10800394(0,0);
    }
    goto LAB_1081a9ac;
  case 0x1e:
    uVar12 = FUN_10824774(param_1,param_2);
    break;
  case 0x1f:
    iVar13 = FUN_108a5aac((char)param_2[1],*(undefined1 *)((int)param_2 + 5),
                          (*param_2 & 0x1fff) >> 8,param_2 + 2,(char)param_2[6]);
    if (iVar13 != 0) {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x34,(char)param_2[1],
                   *(undefined1 *)((int)param_2 + 5));
      goto LAB_1081a994;
    }
    goto LAB_1081a9ac;
  case 0x20:
    uVar12 = FUN_10824978(param_1,param_2);
    break;
  case 0x21:
    uVar12 = FUN_1081f36c(param_1,param_2);
    break;
  case 0x22:
    uVar7 = param_2[1];
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = *(int *)(iVar13 + 0x84);
    uVar3 = *(undefined1 *)((int)param_2 + 5);
    if (*(short *)(iVar13 + 0x12) != 0) {
      uVar10 = FUN_1086b284();
      FUN_1086bb84();
      FUN_1086bac0(2,*(undefined1 *)(iVar13 + 0x19));
      FUN_1086bac0(0x2f,uVar3);
      FUN_1086b9a4(param_1,0x9c);
      FUN_1086b880();
      *(undefined4 *)(param_1 + 0x437c) = uVar10;
    }
    goto LAB_1081ad08;
  case 0x23:
    uVar12 = FUN_1081db1c(param_1,param_2);
    break;
  case 0x24:
    iVar13 = *(int *)(param_1 + 8);
    uVar7 = (*param_2 & 0x1fff) >> 8;
    if (0x17 < uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = *(int *)(iVar13 + 0x84);
    uVar5 = *(undefined2 *)(iVar13 + 0x12);
    if ((char)param_2[1] == '\0') {
      uVar10 = thunk_FUN_0039f0bc();
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x49,*(undefined1 *)(iVar13 + 0x19),uVar10);
    }
    FUN_1086bbe8(param_1,param_2 + 2);
    FUN_1086b140(param_1,iVar13,0,1);
    FUN_1086b098(param_1,iVar13,uVar5);
    goto LAB_1081ad08;
  case 0x25:
    uVar12 = FUN_10820c9c(param_1,param_2);
    break;
  case 0x26:
    uVar7 = param_2[1];
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = *(int *)(iVar13 + 0x84);
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x22,(char)param_2[1],
                 *(undefined1 *)((int)param_2 + 5),*(undefined1 *)((int)param_2 + 6),
                 *(undefined1 *)((int)param_2 + 7),(short)param_2[2]);
    FUN_1082b064(iVar13,iVar13 + (uint)*(byte *)((int)param_2 + 5) * 0xe + 0x116,(short)param_2[2],
                 *(undefined1 *)((int)param_2 + 6),*(char *)((int)param_2 + 7) != '\0');
    goto LAB_1081ad08;
  case 0x27:
    uVar7 = param_2[1];
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = *(int *)(iVar13 + 0x84);
    uVar7 = (uint)*(byte *)((int)param_2 + 5);
    uVar3 = *(undefined1 *)((int)param_2 + 6);
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x29,*(undefined1 *)(iVar13 + 0x19),uVar7,uVar3);
    if (4 < uVar7) {
      FUN_10800394(0,0);
    }
    *(undefined1 *)(iVar13 + uVar7 + 0x16d) = uVar3;
    goto LAB_1081ad08;
  case 0x28:
    uVar12 = FUN_10825eb8(param_1,param_2);
    break;
  case 0x29:
    iVar13 = *(int *)(param_1 + 8);
    uVar7 = (*param_2 & 0x1fff) >> 8;
    if (0x17 < uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = FUN_1086af2c(param_1,*(undefined4 *)(iVar13 + 0x84),1);
    if (iVar13 != 0) {
      *(undefined1 *)(param_1 + 0x18) = 5;
      goto LAB_1081a994;
    }
    goto LAB_1081a9ac;
  case 0x2a:
    uVar12 = FUN_10824288(param_1,param_2);
    break;
  case 0x2b:
    uVar12 = FUN_10822cbc(param_1,param_2);
    break;
  case 0x2c:
    uVar7 = param_2[1];
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = FUN_1086add4(param_1,*(undefined4 *)(iVar13 + 0x84),*(undefined1 *)((int)param_2 + 5));
    if (iVar13 != 0) goto LAB_1081a994;
    goto LAB_1081a9ac;
  case 0x2d:
    uVar12 = FUN_1081e0f8(param_1,param_2);
    break;
  case 0x2e:
    uVar7 = param_2[1];
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    sVar6 = *(short *)((int)param_2 + 6);
    if (sVar6 == 0) {
      sVar6 = 1;
    }
    *(short *)(*(int *)(iVar13 + 0x84) + 0x150) = sVar6;
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x15,(char)param_2[1],
                 *(undefined2 *)((int)param_2 + 6));
    FUN_1086bbe8(param_1,param_2 + 2);
    goto LAB_1081ad08;
  case 0x2f:
    bVar4 = *(byte *)((int)param_2 + 6);
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < bVar4) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)bVar4 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar10 = *(undefined4 *)(iVar13 + 0x84);
    iVar13 = *(int *)(param_1 + 8);
    uVar7 = (*param_2 & 0x1fff) >> 8;
    if (0x17 < uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = FUN_1086a8f8(param_1,*(undefined4 *)(iVar13 + 0x84),uVar10,
                          (*(byte *)((int)param_2 + 7) & 0xf) != 0,auStack_28,(short)param_2[1],
                          (*(byte *)((int)param_2 + 7) & 0xf0) != 0,param_2 + 2);
    goto joined_r0x1081a960;
  case 0x30:
    uVar7 = 0;
    if (*(byte *)(param_1 + 0x108c) != 0) {
      do {
        if (((*(uint *)(param_1 + uVar7 * 0x10 + 0x10c0) & 0xf) != 8) &&
           (*(char *)(*(int *)(param_1 + uVar7 * 0x10 + 0x10bc) + 0x19) == (char)param_2[1])) {
          iVar13 = param_1 + uVar7 * 0x10;
          iVar9 = iVar13 + 0x10bc;
          if ((iVar9 != 0) &&
             ((*(char *)(iVar13 + 0x10c9) == '\0' &&
              (iVar13 = FUN_1086a7fc(param_1,iVar9,0), -1 < iVar13)))) {
            uVar12 = 1;
            FUN_1086bbe8(param_1,param_2 + 2);
            goto LAB_1081ad20;
          }
          break;
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 < *(byte *)(param_1 + 0x108c));
    }
    goto LAB_1081ac70;
  case 0x31:
    iVar13 = FUN_108237fc(param_1,(short)param_2[1]);
joined_r0x1081a960:
    if (iVar13 == 0) goto LAB_1081a9ac;
LAB_1081a994:
    bVar15 = true;
    goto LAB_1081a998;
  case 0x32:
    iVar13 = FUN_1081c950(param_1,(char)param_2[1]);
    if (iVar13 != 0) {
LAB_1081a980:
      FUN_1086bbe8(param_1,param_2 + 2);
      goto LAB_1081a994;
    }
LAB_1081a9ac:
    bVar15 = false;
LAB_1081a998:
    if (bVar15) goto LAB_1081a9a0;
    goto LAB_1081ad30;
  case 0x33:
    uVar12 = FUN_10820f3c(param_1,param_2);
    break;
  case 0x34:
    uVar12 = FUN_1081defc(param_1,param_2);
    break;
  case 0x35:
    iVar13 = FUN_1086c2cc(param_1 + 0x120c,(char)param_2[1]);
    if (iVar13 != 10) {
      uVar7 = param_2[1];
      iVar13 = *(int *)(param_1 + 8);
      if (0x17 < (byte)uVar7) {
        FUN_10800394(0,0);
      }
      iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
      if (*(int *)(iVar13 + 0x84) == 0) {
        FUN_10800394(0,0);
      }
      iVar13 = *(int *)(iVar13 + 0x84);
      if ((*(byte *)(iVar13 + 0x1b) & 0x40) != 0) {
        *(byte *)(iVar13 + 0x1b) = *(byte *)(iVar13 + 0x1b) & 0xbf;
        *(undefined4 *)(iVar13 + 4) = 0;
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x55,(char)param_2[1]);
        goto LAB_1081a980;
      }
    }
    goto LAB_1081ac70;
  case 0x36:
    uVar12 = FUN_10824414(param_1,param_2);
    break;
  case 0x37:
    uVar7 = param_2[1];
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    uVar10 = *(undefined4 *)(iVar13 + 0x84);
    iVar13 = FUN_10874420(uVar10,0xd);
    if ((iVar13 != 0) &&
       (iVar13 = FUN_1081fc20(param_1,uVar10,*(undefined1 *)((int)param_2 + 5),0), iVar13 != 0))
    goto LAB_1081a980;
    goto LAB_1081ac70;
  case 0x38:
    if (*(char *)((int)param_2 + 0xb) != '\0') {
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x62,0);
    }
    FUN_1086a6e0(param_1,(short)param_2[1],*(undefined1 *)((int)param_2 + 6),
                 *(undefined1 *)((int)param_2 + 7),*(char *)((int)param_2 + 10) != '\0',
                 (short)param_2[2]);
    FUN_1086bbe8(param_1,param_2 + 3);
    goto LAB_1081ad08;
  case 0x39:
    uVar10 = *(undefined4 *)(param_1 + 0xc);
    uVar11 = 0x62;
LAB_10819890:
    FUN_1086c22c(uVar10,uVar11);
    goto LAB_1081ad08;
  case 0x3a:
    uVar7 = param_2[1];
    iVar13 = *(int *)(param_1 + 8);
    if (0x17 < (byte)uVar7) {
      FUN_10800394(0,0);
    }
    iVar13 = iVar13 + (uint)(byte)uVar7 * 4;
    if (*(int *)(iVar13 + 0x84) == 0) {
      FUN_10800394(0,0);
    }
    iVar13 = *(int *)(iVar13 + 0x84);
    iVar9 = FUN_10874420(iVar13,0xd);
    if (((iVar9 != 0) && ((*(byte *)(iVar13 + 0x1b) & 0x20) == 0)) &&
       (*(char *)((int)param_2 + 5) != *(char *)(iVar13 + 0x153))) {
      if ((*param_2 & 0x800000) != 0) {
        FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x66,*(undefined1 *)(iVar13 + 0x19));
      }
      FUN_1086fff0(iVar13,*(undefined1 *)((int)param_2 + 5));
      FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),0x5b,(char)param_2[1],
                   *(undefined1 *)((int)param_2 + 5));
      FUN_1086bbe8(param_1,param_2 + 2);
      if ((*param_2 & 0x800000) != 0) {
        uVar10 = *(undefined4 *)(param_1 + 0xc);
        goto LAB_1081988c;
      }
      goto LAB_1081ad08;
    }
LAB_1081ac70:
    uVar12 = 0;
    break;
  case 0x3b:
    uVar12 = 1;
    *(char *)(*(int *)(param_1 + 0xdb4) + 4) = (char)param_2[1];
    break;
  case 0x3c:
    *(byte *)(*(int *)(param_1 + 0xdb4) + 5) = *(byte *)(*(int *)(param_1 + 0xdb4) + 5) | 1;
    goto LAB_1081ad08;
  case 0x3e:
    if (2 < (*(ushort *)((int)param_2 + 10) & 0x3fff) >> 7) {
      FUN_10800394(0,0);
    }
    FUN_1086c22c(*(undefined4 *)(param_1 + 0xc),100,(char)param_2[1],
                 *(undefined1 *)((int)param_2 + 5),*(ushort *)((int)param_2 + 10) & 0x7f,
                 (short)param_2[2],param_2[3] & 0xffff,param_2[4] & 0xffff);
LAB_1081ad08:
    uVar12 = 1;
  }
LAB_1081ad20:
  if (uVar12 == 0) {
LAB_1081ad30:
    uVar7 = *(uint *)(param_1 + 0x437c) & 0xefffffff;
  }
  else {
LAB_1081a9a0:
    uVar7 = *(uint *)(param_1 + 0x437c) | 0x30000000;
  }
  *(uint *)(param_1 + 0x437c) = uVar7 | 0x40000000;
  return;
}

