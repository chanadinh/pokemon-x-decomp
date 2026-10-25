/* Unreviewed Ghidra pseudocode; types require assembly review. */

ulonglong _static_::anon_segment_0_68bbc(undefined4 param_1,uint param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  
  iVar3 = DAT_00168c50;
  uVar5 = *(undefined4 *)(DAT_00168c50 + 4);
  iVar2 = FUN_001762b4(*(undefined4 *)(DAT_00168c50 + 8),param_1,uVar5);
  if (iVar2 == 0) {
    FUN_00174f30(uVar5,param_1);
    FUN_00174eb4(*(undefined4 *)(iVar3 + 8),uVar5);
  }
  iVar3 = *(int *)(iVar3 + 4);
  switch(param_2) {
  case 0:
    uVar4 = (uint)**(byte **)(iVar3 + 8);
    goto LAB_001763c4;
  case 1:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 1));
  case 2:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 2));
  case 3:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 3));
  case 4:
    uVar4 = (uint)*(byte *)(*(int *)(iVar3 + 8) + 4);
    if (uVar4 == 0x65) {
      uVar4 = 100;
    }
    return CONCAT44(param_2,uVar4);
  case 5:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 5));
  case 6:
    return CONCAT44(param_2,(int)*(char *)(*(int *)(iVar3 + 8) + 6));
  case 7:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 0xe));
  case 8:
    return CONCAT44(param_2,(uint)(*(byte *)(*(int *)(iVar3 + 8) + 7) >> 4));
  case 9:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 7)) & 0xffffffff0000000f;
  case 10:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 0xf));
  case 0xb:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(iVar3 + 8) + 8));
  case 0xc:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 10));
  case 0xd:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 0xb));
  case 0xe:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 0xc));
  case 0xf:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 0xd));
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x16:
  case 0x17:
  case 0x18:
    return CONCAT44(param_2 + 5,(uint)*(byte *)(*(int *)(iVar3 + 8) + param_2 + 5));
  case 0x13:
  case 0x14:
  case 0x15:
    return CONCAT44(param_2 + 5,(int)*(char *)(*(int *)(iVar3 + 8) + param_2 + 5));
  case 0x19:
    cVar1 = *(char *)(*(int *)(iVar3 + 8) + 0x12);
    break;
  case 0x1a:
    cVar1 = *(char *)(*(int *)(iVar3 + 8) + 0x13);
    break;
  case 0x1b:
    return CONCAT44(param_2,(uint)*(byte *)(*(int *)(iVar3 + 8) + 0x14));
  case 0x1c:
    return CONCAT44(param_2,(uint)*(ushort *)(*(int *)(iVar3 + 8) + 0x10));
  default:
    param_2 = 0;
    goto LAB_00176414;
  case 0x1e:
    cVar1 = *(char *)(*(int *)(iVar3 + 8) + 0x12);
    goto joined_r0x0017651c;
  case 0x1f:
    cVar1 = *(char *)(*(int *)(iVar3 + 8) + 0x13);
joined_r0x0017651c:
    if (cVar1 < 0) {
      return CONCAT44(param_2,-(int)cVar1);
    }
    goto LAB_00176414;
  case 0x20:
    if (*(char *)(*(int *)(iVar3 + 8) + 4) != 'e') goto LAB_00176414;
    uVar4 = 1;
    goto LAB_001763c4;
  }
  uVar4 = (uint)cVar1;
  if (0 < (int)uVar4) {
LAB_001763c4:
    return CONCAT44(param_2,uVar4);
  }
LAB_00176414:
  return (ulonglong)param_2 << 0x20;
}

